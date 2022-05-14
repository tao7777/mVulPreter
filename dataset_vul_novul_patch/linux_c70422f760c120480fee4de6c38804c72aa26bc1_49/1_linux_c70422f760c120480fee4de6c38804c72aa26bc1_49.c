void svc_rdma_wc_write(struct ib_cq *cq, struct ib_wc *wc)
{
	struct ib_cqe *cqe = wc->wr_cqe;
	struct svc_rdma_op_ctxt *ctxt;
	svc_rdma_send_wc_common_put(cq, wc, "write");
	ctxt = container_of(cqe, struct svc_rdma_op_ctxt, cqe);
	svc_rdma_unmap_dma(ctxt);
	svc_rdma_put_context(ctxt, 0);
}
