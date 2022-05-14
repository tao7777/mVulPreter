static void userfaultfd_event_wait_completion(struct userfaultfd_ctx *ctx,
					      struct userfaultfd_wait_queue *ewq)
{
	if (WARN_ON_ONCE(current->flags & PF_EXITING))
		goto out;

	ewq->ctx = ctx;
	init_waitqueue_entry(&ewq->wq, current);

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
 		if (ACCESS_ONCE(ctx->released) ||
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

				userfaultfd_ctx_put(new);
			}
			break;
		}

		spin_unlock(&ctx->event_wqh.lock);

		wake_up_poll(&ctx->fd_wqh, POLLIN);
		schedule();

		spin_lock(&ctx->event_wqh.lock);
	}
	__set_current_state(TASK_RUNNING);
	spin_unlock(&ctx->event_wqh.lock);

	/*
	 * ctx may go away after this if the userfault pseudo fd is
	 * already released.
	 */
out:
	userfaultfd_ctx_put(ctx);
}
