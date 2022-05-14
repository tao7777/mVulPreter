static int __kprobes perf_event_nmi_handler(struct notifier_block *self,
					    unsigned long cmd, void *__args)
{
	struct die_args *args = __args;
	struct perf_sample_data data;
	struct cpu_hw_events *cpuc;
	struct pt_regs *regs;
	int i;

	if (!atomic_read(&active_events))
		return NOTIFY_DONE;

	switch (cmd) {
	case DIE_NMI:
		break;

	default:
		return NOTIFY_DONE;
	}

	regs = args->regs;

	perf_sample_data_init(&data, 0);

	cpuc = &__get_cpu_var(cpu_hw_events);

	/* If the PMU has the TOE IRQ enable bits, we need to do a
	 * dummy write to the %pcr to clear the overflow bits and thus
	 * the interrupt.
	 *
	 * Do this before we peek at the counters to determine
	 * overflow so we don't lose any events.
	 */
	if (sparc_pmu->irq_bit)
		pcr_ops->write(cpuc->pcr);

	for (i = 0; i < cpuc->n_events; i++) {
		struct perf_event *event = cpuc->event[i];
		int idx = cpuc->current_idx[i];
		struct hw_perf_event *hwc;
		u64 val;

		hwc = &event->hw;
		val = sparc_perf_event_update(event, hwc, idx);
		if (val & (1ULL << 31))
			continue;

		data.period = event->hw.last_period;
 		if (!sparc_perf_event_set_period(event, hwc, idx))
 			continue;
 
		if (perf_event_overflow(event, &data, regs))
 			sparc_pmu_stop(event, 0);
 	}
 
	return NOTIFY_STOP;
}
