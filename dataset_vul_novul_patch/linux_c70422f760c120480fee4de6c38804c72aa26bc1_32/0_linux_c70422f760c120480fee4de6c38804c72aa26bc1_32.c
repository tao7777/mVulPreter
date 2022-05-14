static int send_reply(struct svcxprt_rdma *rdma,
static int svc_rdma_send_reply_msg(struct svcxprt_rdma *rdma,
				   __be32 *rdma_argp, __be32 *rdma_resp,
				   struct svc_rqst *rqstp,
				   __be32 *wr_lst, __be32 *rp_ch)
 {
 	struct svc_rdma_op_ctxt *ctxt;
	u32 inv_rkey;
	int ret;

	dprintk("svcrdma: sending %s reply: head=%zu, pagelen=%u, tail=%zu\n",
		(rp_ch ? "RDMA_NOMSG" : "RDMA_MSG"),
		rqstp->rq_res.head[0].iov_len,
		rqstp->rq_res.page_len,
		rqstp->rq_res.tail[0].iov_len);

 	ctxt = svc_rdma_get_context(rdma);
 
	ret = svc_rdma_map_reply_hdr(rdma, ctxt, rdma_resp,
				     svc_rdma_reply_hdr_len(rdma_resp));
	if (ret < 0)
 		goto err;
 
	if (!rp_ch) {
		ret = svc_rdma_map_reply_msg(rdma, ctxt,
					     &rqstp->rq_res, wr_lst);
		if (ret < 0)
 			goto err;
 	}

	svc_rdma_save_io_pages(rqstp, ctxt);

	inv_rkey = 0;
	if (rdma->sc_snd_w_inv)
		inv_rkey = svc_rdma_get_inv_rkey(rdma_argp, wr_lst, rp_ch);
	ret = svc_rdma_post_send_wr(rdma, ctxt, 1 + ret, inv_rkey);
	if (ret)
 		goto err;
 
	return 0;

err:
	pr_err("svcrdma: failed to post Send WR (%d)\n", ret);
	svc_rdma_unmap_dma(ctxt);
	svc_rdma_put_context(ctxt, 1);
	return ret;
}

/* Given the client-provided Write and Reply chunks, the server was not
 * able to form a complete reply. Return an RDMA_ERROR message so the
 * client can retire this RPC transaction. As above, the Send completion
 * routine releases payload pages that were part of a previous RDMA Write.
 *
 * Remote Invalidation is skipped for simplicity.
 */
static int svc_rdma_send_error_msg(struct svcxprt_rdma *rdma,
				   __be32 *rdma_resp, struct svc_rqst *rqstp)
{
	struct svc_rdma_op_ctxt *ctxt;
	__be32 *p;
	int ret;

	ctxt = svc_rdma_get_context(rdma);

	/* Replace the original transport header with an
	 * RDMA_ERROR response. XID etc are preserved.
 	 */
	p = rdma_resp + 3;
	*p++ = rdma_error;
	*p   = err_chunk;
 
	ret = svc_rdma_map_reply_hdr(rdma, ctxt, rdma_resp, 20);
	if (ret < 0)
 		goto err;
 
	svc_rdma_save_io_pages(rqstp, ctxt);

	ret = svc_rdma_post_send_wr(rdma, ctxt, 1 + ret, 0);
 	if (ret)
 		goto err;
 
 	return 0;
 
err:
	pr_err("svcrdma: failed to post Send WR (%d)\n", ret);
 	svc_rdma_unmap_dma(ctxt);
 	svc_rdma_put_context(ctxt, 1);
 	return ret;
}
