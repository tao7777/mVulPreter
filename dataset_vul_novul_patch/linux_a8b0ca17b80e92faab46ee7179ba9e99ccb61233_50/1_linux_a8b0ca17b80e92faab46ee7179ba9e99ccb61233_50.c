static void kgdb_hw_overflow_handler(struct perf_event *event, int nmi,
 		struct perf_sample_data *data, struct pt_regs *regs)
 {
 	struct task_struct *tsk = current;
	int i;

	for (i = 0; i < 4; i++)
		if (breakinfo[i].enabled)
			tsk->thread.debugreg6 |= (DR_TRAP0 << i);
}
