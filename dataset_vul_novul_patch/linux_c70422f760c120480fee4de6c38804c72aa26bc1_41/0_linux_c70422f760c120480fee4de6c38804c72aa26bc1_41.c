static void __svc_rdma_free(struct work_struct *work)
{
	struct svcxprt_rdma *rdma =
		container_of(work, struct svcxprt_rdma, sc_work);
	struct svc_xprt *xprt = &rdma->sc_xprt;

	dprintk("svcrdma: %s(%p)\n", __func__, rdma);

	if (rdma->sc_qp && !IS_ERR(rdma->sc_qp))
		ib_drain_qp(rdma->sc_qp);

	/* We should only be called from kref_put */
	if (kref_read(&xprt->xpt_ref) != 0)
		pr_err("svcrdma: sc_xprt still in use? (%d)\n",
		       kref_read(&xprt->xpt_ref));

	/*
	 * Destroy queued, but not processed read completions. Note
	 * that this cleanup has to be done before destroying the
	 * cm_id because the device ptr is needed to unmap the dma in
	 * svc_rdma_put_context.
	 */
	while (!list_empty(&rdma->sc_read_complete_q)) {
		struct svc_rdma_op_ctxt *ctxt;
		ctxt = list_first_entry(&rdma->sc_read_complete_q,
					struct svc_rdma_op_ctxt, list);
		list_del(&ctxt->list);
		svc_rdma_put_context(ctxt, 1);
	}

	/* Destroy queued, but not processed recv completions */
	while (!list_empty(&rdma->sc_rq_dto_q)) {
		struct svc_rdma_op_ctxt *ctxt;
		ctxt = list_first_entry(&rdma->sc_rq_dto_q,
					struct svc_rdma_op_ctxt, list);
		list_del(&ctxt->list);
		svc_rdma_put_context(ctxt, 1);
	}

	/* Warn if we leaked a resource or under-referenced */
	if (rdma->sc_ctxt_used != 0)
		pr_err("svcrdma: ctxt still in use? (%d)\n",
		       rdma->sc_ctxt_used);

	/* Final put of backchannel client transport */
	if (xprt->xpt_bc_xprt) {
		xprt_put(xprt->xpt_bc_xprt);
		xprt->xpt_bc_xprt = NULL;
 	}
 
 	rdma_dealloc_frmr_q(rdma);
	svc_rdma_destroy_rw_ctxts(rdma);
 	svc_rdma_destroy_ctxts(rdma);
 
 	/* Destroy the QP if present (not a listener) */
 	if (rdma->sc_qp && !IS_ERR(rdma->sc_qp))
		ib_destroy_qp(rdma->sc_qp);

	if (rdma->sc_sq_cq && !IS_ERR(rdma->sc_sq_cq))
		ib_free_cq(rdma->sc_sq_cq);

	if (rdma->sc_rq_cq && !IS_ERR(rdma->sc_rq_cq))
		ib_free_cq(rdma->sc_rq_cq);

	if (rdma->sc_pd && !IS_ERR(rdma->sc_pd))
		ib_dealloc_pd(rdma->sc_pd);

	/* Destroy the CM ID */
	rdma_destroy_id(rdma->sc_cm_id);

	kfree(rdma);
}
