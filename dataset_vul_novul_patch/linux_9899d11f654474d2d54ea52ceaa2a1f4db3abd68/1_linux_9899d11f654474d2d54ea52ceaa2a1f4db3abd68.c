void set_task_blockstep(struct task_struct *task, bool on)
{
	unsigned long debugctl;

	/*
 	 * Ensure irq/preemption can't change debugctl in between.
 	 * Note also that both TIF_BLOCKSTEP and debugctl should
 	 * be changed atomically wrt preemption.
	 * FIXME: this means that set/clear TIF_BLOCKSTEP is simply
	 * wrong if task != current, SIGKILL can wakeup the stopped
	 * tracee and set/clear can play with the running task, this
	 * can confuse the next __switch_to_xtra().
 	 */
 	local_irq_disable();
 	debugctl = get_debugctlmsr();
	if (on) {
		debugctl |= DEBUGCTLMSR_BTF;
		set_tsk_thread_flag(task, TIF_BLOCKSTEP);
	} else {
		debugctl &= ~DEBUGCTLMSR_BTF;
		clear_tsk_thread_flag(task, TIF_BLOCKSTEP);
	}
	if (task == current)
		update_debugctlmsr(debugctl);
	local_irq_enable();
}
