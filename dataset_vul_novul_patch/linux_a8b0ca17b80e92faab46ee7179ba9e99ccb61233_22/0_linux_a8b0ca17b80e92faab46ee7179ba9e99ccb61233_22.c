void ptrace_triggered(struct perf_event *bp, int nmi,
void ptrace_triggered(struct perf_event *bp,
 		      struct perf_sample_data *data, struct pt_regs *regs)
 {
 	struct perf_event_attr attr;

	/*
	 * Disable the breakpoint request here since ptrace has defined a
	 * one-shot behaviour for breakpoint exceptions.
	 */
	attr = bp->attr;
	attr.disabled = true;
	modify_user_hw_breakpoint(bp, &attr);
}
