static struct svcxprt_rdma *rdma_create_xprt(struct svc_serv *serv,
					     int listener)
{
	struct svcxprt_rdma *cma_xprt = kzalloc(sizeof *cma_xprt, GFP_KERNEL);

	if (!cma_xprt)
		return NULL;
	svc_xprt_init(&init_net, &svc_rdma_class, &cma_xprt->sc_xprt, serv);
	INIT_LIST_HEAD(&cma_xprt->sc_accept_q);
	INIT_LIST_HEAD(&cma_xprt->sc_rq_dto_q);
 	INIT_LIST_HEAD(&cma_xprt->sc_read_complete_q);
 	INIT_LIST_HEAD(&cma_xprt->sc_frmr_q);
 	INIT_LIST_HEAD(&cma_xprt->sc_ctxts);
	INIT_LIST_HEAD(&cma_xprt->sc_rw_ctxts);
 	init_waitqueue_head(&cma_xprt->sc_send_wait);
 
 	spin_lock_init(&cma_xprt->sc_lock);
 	spin_lock_init(&cma_xprt->sc_rq_dto_lock);
 	spin_lock_init(&cma_xprt->sc_frmr_q_lock);
 	spin_lock_init(&cma_xprt->sc_ctxt_lock);
	spin_lock_init(&cma_xprt->sc_rw_ctxt_lock);
 
 	/*
 	 * Note that this implies that the underlying transport support
	 * has some form of congestion control (see RFC 7530 section 3.1
	 * paragraph 2). For now, we assume that all supported RDMA
	 * transports are suitable here.
	 */
	set_bit(XPT_CONG_CTRL, &cma_xprt->sc_xprt.xpt_flags);

	if (listener)
		set_bit(XPT_LISTENER, &cma_xprt->sc_xprt.xpt_flags);

	return cma_xprt;
}
