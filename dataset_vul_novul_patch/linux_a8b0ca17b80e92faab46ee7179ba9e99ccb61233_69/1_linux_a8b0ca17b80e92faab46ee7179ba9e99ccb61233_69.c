static void perf_output_wakeup(struct perf_output_handle *handle)
 {
 	atomic_set(&handle->rb->poll, POLL_IN);
 
	if (handle->nmi) {
		handle->event->pending_wakeup = 1;
		irq_work_queue(&handle->event->pending);
	} else
		perf_event_wakeup(handle->event);
 }
