dotraplinkage void do_stack_segment(struct pt_regs *regs, long error_code)
{
	enum ctx_state prev_state;
	prev_state = exception_enter();
	if (notify_die(DIE_TRAP, "stack segment", regs, error_code,
		       X86_TRAP_SS, SIGBUS) != NOTIFY_STOP) {
		preempt_conditional_sti(regs);
		do_trap(X86_TRAP_SS, SIGBUS, "stack segment", regs, error_code, NULL);
		preempt_conditional_cli(regs);
	}
	exception_exit(prev_state);
}
