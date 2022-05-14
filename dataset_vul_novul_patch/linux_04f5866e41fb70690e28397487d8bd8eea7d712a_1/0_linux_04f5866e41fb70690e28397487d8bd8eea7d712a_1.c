static void userfaultfd_event_wait_completion(struct userfaultfd_ctx *ctx,
					      struct userfaultfd_wait_queue *ewq)
{
	struct userfaultfd_ctx *release_new_ctx;

	if (WARN_ON_ONCE(current->flags & PF_EXITING))
		goto out;

	ewq->ctx = ctx;
	init_waitqueue_entry(&ewq->wq, current);
	release_new_ctx = NULL;

	spin_lock(&ctx->event_wqh.lock);
	/*
	 * After the __add_wait_queue the uwq is visible to userland
	 * through poll/read().
	 */
	__add_wait_queue(&ctx->event_wqh, &ewq->wq);
	for (;;) {
		set_current_state(TASK_KILLABLE);
		if (ewq->msg.event == 0)
			break;
		if (READ_ONCE(ctx->released) ||
		    fatal_signal_pending(current)) {
			/*
			 * &ewq->wq may be queued in fork_event, but
			 * __remove_wait_queue ignores the head
			 * parameter. It would be a problem if it
			 * didn't.
			 */
			__remove_wait_queue(&ctx->event_wqh, &ewq->wq);
			if (ewq->msg.event == UFFD_EVENT_FORK) {
				struct userfaultfd_ctx *new;

				new = (struct userfaultfd_ctx *)
					(unsigned long)
					ewq->msg.arg.reserved.reserved1;
				release_new_ctx = new;
			}
			break;
		}

		spin_unlock(&ctx->event_wqh.lock);

		wake_up_poll(&ctx->fd_wqh, EPOLLIN);
		schedule();

		spin_lock(&ctx->event_wqh.lock);
	}
	__set_current_state(TASK_RUNNING);
	spin_unlock(&ctx->event_wqh.lock);

	if (release_new_ctx) {
		struct vm_area_struct *vma;
		struct mm_struct *mm = release_new_ctx->mm;
 
 		/* the various vma->vm_userfaultfd_ctx still points to it */
 		down_write(&mm->mmap_sem);
		/* no task can run (and in turn coredump) yet */
		VM_WARN_ON(!mmget_still_valid(mm));
 		for (vma = mm->mmap; vma; vma = vma->vm_next)
 			if (vma->vm_userfaultfd_ctx.ctx == release_new_ctx) {
 				vma->vm_userfaultfd_ctx = NULL_VM_UFFD_CTX;
				vma->vm_flags &= ~(VM_UFFD_WP | VM_UFFD_MISSING);
			}
		up_write(&mm->mmap_sem);

		userfaultfd_ctx_put(release_new_ctx);
	}

	/*
	 * ctx may go away after this if the userfault pseudo fd is
	 * already released.
	 */
out:
	WRITE_ONCE(ctx->mmap_changing, false);
	userfaultfd_ctx_put(ctx);
}
