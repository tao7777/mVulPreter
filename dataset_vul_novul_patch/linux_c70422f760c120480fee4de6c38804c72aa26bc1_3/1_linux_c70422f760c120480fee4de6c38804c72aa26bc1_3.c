nfs4_callback_svc(void *vrqstp)
{
	int err;
	struct svc_rqst *rqstp = vrqstp;
 
 	set_freezable();
 
	while (!kthread_should_stop()) {
 		/*
 		 * Listen for a request on the socket
 		 */
		err = svc_recv(rqstp, MAX_SCHEDULE_TIMEOUT);
		if (err == -EAGAIN || err == -EINTR)
 			continue;
 		svc_process(rqstp);
 	}
 	return 0;
 }
