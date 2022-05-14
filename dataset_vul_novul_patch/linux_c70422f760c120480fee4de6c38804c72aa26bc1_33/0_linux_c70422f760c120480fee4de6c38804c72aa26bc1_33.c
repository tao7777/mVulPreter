static int send_reply_chunks(struct svcxprt_rdma *xprt,
/* The svc_rqst and all resources it owns are released as soon as
 * svc_rdma_sendto returns. Transfer pages under I/O to the ctxt
 * so they are released by the Send completion handler.
 */
static void svc_rdma_save_io_pages(struct svc_rqst *rqstp,
				   struct svc_rdma_op_ctxt *ctxt)
 {
	int i, pages = rqstp->rq_next_page - rqstp->rq_respages;
 
	ctxt->count += pages;
	for (i = 0; i < pages; i++) {
		ctxt->pages[i + 1] = rqstp->rq_respages[i];
		rqstp->rq_respages[i] = NULL;
 	}
	rqstp->rq_next_page = rqstp->rq_respages + 1;
}
 
/**
 * svc_rdma_post_send_wr - Set up and post one Send Work Request
 * @rdma: controlling transport
 * @ctxt: op_ctxt for transmitting the Send WR
 * @num_sge: number of SGEs to send
 * @inv_rkey: R_key argument to Send With Invalidate, or zero
 *
 * Returns:
 *	%0 if the Send* was posted successfully,
 *	%-ENOTCONN if the connection was lost or dropped,
 *	%-EINVAL if there was a problem with the Send we built,
 *	%-ENOMEM if ib_post_send failed.
 */
int svc_rdma_post_send_wr(struct svcxprt_rdma *rdma,
			  struct svc_rdma_op_ctxt *ctxt, int num_sge,
			  u32 inv_rkey)
{
	struct ib_send_wr *send_wr = &ctxt->send_wr;
 
	dprintk("svcrdma: posting Send WR with %u sge(s)\n", num_sge);

	send_wr->next = NULL;
	ctxt->cqe.done = svc_rdma_wc_send;
	send_wr->wr_cqe = &ctxt->cqe;
	send_wr->sg_list = ctxt->sge;
	send_wr->num_sge = num_sge;
	send_wr->send_flags = IB_SEND_SIGNALED;
	if (inv_rkey) {
		send_wr->opcode = IB_WR_SEND_WITH_INV;
		send_wr->ex.invalidate_rkey = inv_rkey;
	} else {
		send_wr->opcode = IB_WR_SEND;
	}

	return svc_rdma_send(rdma, send_wr);
 }
