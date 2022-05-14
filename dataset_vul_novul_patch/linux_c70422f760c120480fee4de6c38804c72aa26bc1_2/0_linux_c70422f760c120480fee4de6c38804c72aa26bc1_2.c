nfs41_callback_svc(void *vrqstp)
{
	struct svc_rqst *rqstp = vrqstp;
	struct svc_serv *serv = rqstp->rq_server;
	struct rpc_rqst *req;
	int error;
	DEFINE_WAIT(wq);
 
 	set_freezable();
 
	while (!kthread_freezable_should_stop(NULL)) {

		if (signal_pending(current))
			flush_signals(current);
 
 		prepare_to_wait(&serv->sv_cb_waitq, &wq, TASK_INTERRUPTIBLE);
 		spin_lock_bh(&serv->sv_cb_lock);
		if (!list_empty(&serv->sv_cb_list)) {
			req = list_first_entry(&serv->sv_cb_list,
					struct rpc_rqst, rq_bc_list);
			list_del(&req->rq_bc_list);
			spin_unlock_bh(&serv->sv_cb_lock);
			finish_wait(&serv->sv_cb_waitq, &wq);
			dprintk("Invoking bc_svc_process()\n");
			error = bc_svc_process(serv, req, rqstp);
			dprintk("bc_svc_process() returned w/ error code= %d\n",
 				error);
 		} else {
 			spin_unlock_bh(&serv->sv_cb_lock);
			if (!kthread_should_stop())
				schedule();
 			finish_wait(&serv->sv_cb_waitq, &wq);
 		}
 	}
	svc_exit_thread(rqstp);
	module_put_and_exit(0);
 	return 0;
 }
