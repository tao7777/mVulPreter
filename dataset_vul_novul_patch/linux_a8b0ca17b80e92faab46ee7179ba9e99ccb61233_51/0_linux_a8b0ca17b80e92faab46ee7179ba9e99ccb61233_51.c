static void ptrace_triggered(struct perf_event *bp, int nmi,
static void ptrace_triggered(struct perf_event *bp,
 			     struct perf_sample_data *data,
 			     struct pt_regs *regs)
 {
	int i;
	struct thread_struct *thread = &(current->thread);

	/*
	 * Store in the virtual DR6 register the fact that the breakpoint
	 * was hit so the thread's debugger will see it.
	 */
	for (i = 0; i < HBP_NUM; i++) {
		if (thread->ptrace_bps[i] == bp)
			break;
	}

	thread->debugreg6 |= (DR_TRAP0 << i);
}
