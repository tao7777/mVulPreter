static void watchdog_overflow_callback(struct perf_event *event, int nmi,
 		 struct perf_sample_data *data,
 		 struct pt_regs *regs)
 {
	/* Ensure the watchdog never gets throttled */
	event->hw.interrupts = 0;

	if (__this_cpu_read(watchdog_nmi_touch) == true) {
		__this_cpu_write(watchdog_nmi_touch, false);
		return;
	}

	/* check for a hardlockup
	 * This is done by making sure our timer interrupt
	 * is incrementing.  The timer interrupt should have
	 * fired multiple times before we overflow'd.  If it hasn't
	 * then this is a good indication the cpu is stuck
	 */
	if (is_hardlockup()) {
		int this_cpu = smp_processor_id();

		/* only print hardlockups once */
		if (__this_cpu_read(hard_watchdog_warn) == true)
			return;

		if (hardlockup_panic)
			panic("Watchdog detected hard LOCKUP on cpu %d", this_cpu);
		else
			WARN(1, "Watchdog detected hard LOCKUP on cpu %d", this_cpu);

		__this_cpu_write(hard_watchdog_warn, true);
		return;
	}

	__this_cpu_write(hard_watchdog_warn, false);
	return;
}
