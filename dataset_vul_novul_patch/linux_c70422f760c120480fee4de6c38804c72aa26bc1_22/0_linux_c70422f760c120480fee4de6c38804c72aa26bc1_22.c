 static int svc_rdma_bc_sendto(struct svcxprt_rdma *rdma,
 			      struct rpc_rqst *rqst)
 {
 	struct svc_rdma_op_ctxt *ctxt;
 	int ret;
 
	ctxt = svc_rdma_get_context(rdma);

	/* rpcrdma_bc_send_request builds the transport header and
	 * the backchannel RPC message in the same buffer. Thus only
	 * one SGE is needed to send both.
	 */
	ret = svc_rdma_map_reply_hdr(rdma, ctxt, rqst->rq_buffer,
				     rqst->rq_snd_buf.len);
	if (ret < 0)
 		goto out_err;
 
 	ret = svc_rdma_repost_recv(rdma, GFP_NOIO);
 	if (ret)
 		goto out_err;
 
	ret = svc_rdma_post_send_wr(rdma, ctxt, 1, 0);
	if (ret)
 		goto out_unmap;
 
 out_err:
 	dprintk("svcrdma: %s returns %d\n", __func__, ret);
 	return ret;
 
 out_unmap:
 	svc_rdma_unmap_dma(ctxt);
 	svc_rdma_put_context(ctxt, 1);
	ret = -EIO;
 	goto out_err;
 }
