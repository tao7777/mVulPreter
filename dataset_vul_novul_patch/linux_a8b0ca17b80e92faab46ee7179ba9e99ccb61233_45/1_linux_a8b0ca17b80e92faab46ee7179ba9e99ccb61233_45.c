static int x86_pmu_handle_irq(struct pt_regs *regs)
{
	struct perf_sample_data data;
	struct cpu_hw_events *cpuc;
	struct perf_event *event;
	int idx, handled = 0;
	u64 val;

	perf_sample_data_init(&data, 0);

	cpuc = &__get_cpu_var(cpu_hw_events);

	/*
	 * Some chipsets need to unmask the LVTPC in a particular spot
	 * inside the nmi handler.  As a result, the unmasking was pushed
	 * into all the nmi handlers.
	 *
	 * This generic handler doesn't seem to have any issues where the
	 * unmasking occurs so it was left at the top.
	 */
	apic_write(APIC_LVTPC, APIC_DM_NMI);

	for (idx = 0; idx < x86_pmu.num_counters; idx++) {
		if (!test_bit(idx, cpuc->active_mask)) {
			/*
			 * Though we deactivated the counter some cpus
			 * might still deliver spurious interrupts still
			 * in flight. Catch them:
			 */
			if (__test_and_clear_bit(idx, cpuc->running))
				handled++;
			continue;
		}

		event = cpuc->events[idx];

		val = x86_perf_event_update(event);
		if (val & (1ULL << (x86_pmu.cntval_bits - 1)))
			continue;

		/*
		 * event overflow
		 */
		handled++;
		data.period	= event->hw.last_period;

 		if (!x86_perf_event_set_period(event))
 			continue;
 
		if (perf_event_overflow(event, 1, &data, regs))
 			x86_pmu_stop(event, 0);
 	}
 
	if (handled)
		inc_irq_stat(apic_perf_irqs);

	return handled;
}
