lockd(void *vrqstp)
 {
 	int		err = 0;
 	struct svc_rqst *rqstp = vrqstp;
 
 	/* try_to_freeze() is called from svc_recv() */
 	set_freezable();

	/* Allow SIGKILL to tell lockd to drop all of its locks */
	allow_signal(SIGKILL);

	dprintk("NFS locking service started (ver " LOCKD_VERSION ").\n");

	/*
	 * The main request loop. We don't terminate until the last
	 * NFS mount or NFS daemon has gone away.
	 */
	while (!kthread_should_stop()) {
		long timeout = MAX_SCHEDULE_TIMEOUT;
		RPC_IFDEBUG(char buf[RPC_MAX_ADDRBUFLEN]);

		/* update sv_maxconn if it has changed */
		rqstp->rq_server->sv_maxconn = nlm_max_connections;

		if (signalled()) {
			flush_signals(current);
			restart_grace();
			continue;
		}

		timeout = nlmsvc_retry_blocked();

		/*
		 * Find a socket with data available and call its
		 * recvfrom routine.
		 */
		err = svc_recv(rqstp, timeout);
		if (err == -EAGAIN || err == -EINTR)
			continue;
		dprintk("lockd: request from %s\n",
				svc_print_addr(rqstp, buf, sizeof(buf)));

		svc_process(rqstp);
	}
	flush_signals(current);
 	if (nlmsvc_ops)
 		nlmsvc_invalidate_all();
 	nlm_shutdown_hosts();
 	return 0;
 }
