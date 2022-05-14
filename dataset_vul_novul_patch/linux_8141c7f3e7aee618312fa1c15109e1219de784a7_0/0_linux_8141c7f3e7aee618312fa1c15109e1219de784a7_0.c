void mm_release(struct task_struct *tsk, struct mm_struct *mm)
 {
 	struct completion *vfork_done = tsk->vfork_done;
 
	/* Get rid of any futexes when releasing the mm */
#ifdef CONFIG_FUTEX
	if (unlikely(tsk->robust_list))
		exit_robust_list(tsk);
#ifdef CONFIG_COMPAT
	if (unlikely(tsk->compat_robust_list))
		compat_exit_robust_list(tsk);
#endif
#endif

 	/* Get rid of any cached register state */
 	deactivate_mm(tsk, mm);
 
	/* notify parent sleeping on vfork() */
	if (vfork_done) {
		tsk->vfork_done = NULL;
		complete(vfork_done);
	}

	/*
	 * If we're exiting normally, clear a user-space tid field if
	 * requested.  We leave this alone when dying by signal, to leave
	 * the value intact in a core dump, and to save the unnecessary
	 * trouble otherwise.  Userland only wants this done for a sys_exit.
	 */
	if (tsk->clear_child_tid
	    && !(tsk->flags & PF_SIGNALED)
	    && atomic_read(&mm->mm_users) > 1) {
		u32 __user * tidptr = tsk->clear_child_tid;
		tsk->clear_child_tid = NULL;

		/*
		 * We don't check the error code - if userspace has
		 * not set up a proper pointer then tough luck.
		 */
		put_user(0, tidptr);
		sys_futex(tidptr, FUTEX_WAKE, 1, NULL, NULL, 0);
	}
}
