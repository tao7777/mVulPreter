svc_set_num_threads(struct svc_serv *serv, struct svc_pool *pool, int nrservs)
/* create new threads */
static int
svc_start_kthreads(struct svc_serv *serv, struct svc_pool *pool, int nrservs)
 {
 	struct svc_rqst	*rqstp;
 	struct task_struct *task;
 	struct svc_pool *chosen_pool;
 	unsigned int state = serv->sv_nrthreads-1;
 	int node;
 
	do {
 		nrservs--;
 		chosen_pool = choose_pool(serv, pool, &state);
 
 		node = svc_pool_map_get_node(chosen_pool->sp_id);
 		rqstp = svc_prepare_thread(serv, chosen_pool, node);
		if (IS_ERR(rqstp))
			return PTR_ERR(rqstp);
 
 		__module_get(serv->sv_ops->svo_module);
 		task = kthread_create_on_node(serv->sv_ops->svo_function, rqstp,
 					      node, "%s", serv->sv_name);
 		if (IS_ERR(task)) {
 			module_put(serv->sv_ops->svo_module);
 			svc_exit_thread(rqstp);
			return PTR_ERR(task);
 		}
 
 		rqstp->rq_task = task;
		if (serv->sv_nrpools > 1)
			svc_pool_map_set_cpumask(task, chosen_pool->sp_id);
 
 		svc_sock_update_bufs(serv);
 		wake_up_process(task);
	} while (nrservs > 0);

	return 0;
}


/* destroy old threads */
static int
svc_signal_kthreads(struct svc_serv *serv, struct svc_pool *pool, int nrservs)
{
	struct task_struct *task;
	unsigned int state = serv->sv_nrthreads-1;

 	/* destroy old threads */
	do {
		task = choose_victim(serv, pool, &state);
		if (task == NULL)
			break;
 		send_sig(SIGINT, task, 1);
 		nrservs++;
	} while (nrservs < 0);

	return 0;
}

/*
 * Create or destroy enough new threads to make the number
 * of threads the given number.  If `pool' is non-NULL, applies
 * only to threads in that pool, otherwise round-robins between
 * all pools.  Caller must ensure that mutual exclusion between this and
 * server startup or shutdown.
 *
 * Destroying threads relies on the service threads filling in
 * rqstp->rq_task, which only the nfs ones do.  Assumes the serv
 * has been created using svc_create_pooled().
 *
 * Based on code that used to be in nfsd_svc() but tweaked
 * to be pool-aware.
 */
int
svc_set_num_threads(struct svc_serv *serv, struct svc_pool *pool, int nrservs)
{
	if (pool == NULL) {
		/* The -1 assumes caller has done a svc_get() */
		nrservs -= (serv->sv_nrthreads-1);
	} else {
		spin_lock_bh(&pool->sp_lock);
		nrservs -= pool->sp_nrthreads;
		spin_unlock_bh(&pool->sp_lock);
 	}
 
	if (nrservs > 0)
		return svc_start_kthreads(serv, pool, nrservs);
	if (nrservs < 0)
		return svc_signal_kthreads(serv, pool, nrservs);
	return 0;
 }
