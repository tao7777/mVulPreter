nfs4_callback_svc(void *vrqstp)
{
	int err;
	struct svc_rqst *rqstp = vrqstp;
 
 	set_freezable();
 
	while (!kthread_freezable_should_stop(NULL)) {

		if (signal_pending(current))
			flush_signals(current);
 		/*
 		 * Listen for a request on the socket
 		 */
		err = svc_recv(rqstp, MAX_SCHEDULE_TIMEOUT);
		if (err == -EAGAIN || err == -EINTR)
 			continue;
 		svc_process(rqstp);
 	}
	svc_exit_thread(rqstp);
	module_put_and_exit(0);
 	return 0;
 }
