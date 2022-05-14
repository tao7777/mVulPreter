static inline int may_ptrace_stop(void)
{
	if (!likely(current->ptrace))
		return 0;
	/*
	 * Are we in the middle of do_coredump?
	 * If so and our tracer is also part of the coredump stopping
	 * is a deadlock situation, and pointless because our tracer
	 * is dead so don't allow us to stop.
 	 * If SIGKILL was already sent before the caller unlocked
 	 * ->siglock we must see ->core_state != NULL. Otherwise it
 	 * is safe to enter schedule().
	 *
	 * This is almost outdated, a task with the pending SIGKILL can't
	 * block in TASK_TRACED. But PTRACE_EVENT_EXIT can be reported
	 * after SIGKILL was already dequeued.
 	 */
 	if (unlikely(current->mm->core_state) &&
 	    unlikely(current->mm == current->parent->mm))
		return 0;

	return 1;
}
