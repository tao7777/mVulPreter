void set_task_blockstep(struct task_struct *task, bool on)
{
	unsigned long debugctl;

	/*
 	 * Ensure irq/preemption can't change debugctl in between.
 	 * Note also that both TIF_BLOCKSTEP and debugctl should
 	 * be changed atomically wrt preemption.
	 *
	 * NOTE: this means that set/clear TIF_BLOCKSTEP is only safe if
	 * task is current or it can't be running, otherwise we can race
	 * with __switch_to_xtra(). We rely on ptrace_freeze_traced() but
	 * PTRACE_KILL is not safe.
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
