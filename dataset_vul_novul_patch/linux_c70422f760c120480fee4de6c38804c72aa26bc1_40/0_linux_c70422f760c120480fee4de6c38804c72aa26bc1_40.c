 int svc_rdma_sendto(struct svc_rqst *rqstp)
 {
 	struct svc_xprt *xprt = rqstp->rq_xprt;
 	struct svcxprt_rdma *rdma =
 		container_of(xprt, struct svcxprt_rdma, sc_xprt);
	__be32 *p, *rdma_argp, *rdma_resp, *wr_lst, *rp_ch;
	struct xdr_buf *xdr = &rqstp->rq_res;
 	struct page *res_page;
	int ret;
 
	/* Find the call's chunk lists to decide how to send the reply.
	 * Receive places the Call's xprt header at the start of page 0.
 	 */
 	rdma_argp = page_address(rqstp->rq_pages[0]);
	svc_rdma_get_write_arrays(rdma_argp, &wr_lst, &rp_ch);
 
	dprintk("svcrdma: preparing response for XID 0x%08x\n",
		be32_to_cpup(rdma_argp));
 
 	/* Create the RDMA response header. xprt->xpt_mutex,
 	 * acquired in svc_send(), serializes RPC replies. The
	 * code path below that inserts the credit grant value
	 * into each transport header runs only inside this
	 * critical section.
	 */
	ret = -ENOMEM;
	res_page = alloc_page(GFP_KERNEL);
	if (!res_page)
 		goto err0;
 	rdma_resp = page_address(res_page);
 
	p = rdma_resp;
	*p++ = *rdma_argp;
	*p++ = *(rdma_argp + 1);
 	*p++ = rdma->sc_fc_credits;
	*p++ = rp_ch ? rdma_nomsg : rdma_msg;
 
 	/* Start with empty chunks */
 	*p++ = xdr_zero;
 	*p++ = xdr_zero;
 	*p   = xdr_zero;
 
	if (wr_lst) {
		/* XXX: Presume the client sent only one Write chunk */
		ret = svc_rdma_send_write_chunk(rdma, wr_lst, xdr);
 		if (ret < 0)
			goto err2;
		svc_rdma_xdr_encode_write_list(rdma_resp, wr_lst, ret);
 	}
	if (rp_ch) {
		ret = svc_rdma_send_reply_chunk(rdma, rp_ch, wr_lst, xdr);
 		if (ret < 0)
			goto err2;
		svc_rdma_xdr_encode_reply_chunk(rdma_resp, rp_ch, ret);
 	}
 
 	ret = svc_rdma_post_recv(rdma, GFP_KERNEL);
 	if (ret)
 		goto err1;
	ret = svc_rdma_send_reply_msg(rdma, rdma_argp, rdma_resp, rqstp,
				      wr_lst, rp_ch);
 	if (ret < 0)
 		goto err0;
	return 0;
 
 err2:
	if (ret != -E2BIG)
		goto err1;

	ret = svc_rdma_post_recv(rdma, GFP_KERNEL);
	if (ret)
		goto err1;
	ret = svc_rdma_send_error_msg(rdma, rdma_resp, rqstp);
	if (ret < 0)
		goto err0;
	return 0;
 
  err1:
 	put_page(res_page);
  err0:
 	pr_err("svcrdma: Could not send reply, err=%d. Closing transport.\n",
 	       ret);
	set_bit(XPT_CLOSE, &xprt->xpt_flags);
 	return -ENOTCONN;
 }
