 int svc_rdma_sendto(struct svc_rqst *rqstp)
 {
 	struct svc_xprt *xprt = rqstp->rq_xprt;
 	struct svcxprt_rdma *rdma =
 		container_of(xprt, struct svcxprt_rdma, sc_xprt);
	struct rpcrdma_msg *rdma_argp;
	struct rpcrdma_msg *rdma_resp;
	struct rpcrdma_write_array *wr_ary, *rp_ary;
	int ret;
	int inline_bytes;
 	struct page *res_page;
	struct svc_rdma_req_map *vec;
	u32 inv_rkey;
	__be32 *p;
	dprintk("svcrdma: sending response for rqstp=%p\n", rqstp);
 
	/* Get the RDMA request header. The receive logic always
	 * places this at the start of page 0.
 	 */
 	rdma_argp = page_address(rqstp->rq_pages[0]);
	svc_rdma_get_write_arrays(rdma_argp, &wr_ary, &rp_ary);
	inv_rkey = 0;
	if (rdma->sc_snd_w_inv)
		inv_rkey = svc_rdma_get_inv_rkey(rdma_argp, wr_ary, rp_ary);
 
	/* Build an req vec for the XDR */
	vec = svc_rdma_get_req_map(rdma);
	ret = svc_rdma_map_xdr(rdma, &rqstp->rq_res, vec, wr_ary != NULL);
	if (ret)
		goto err0;
	inline_bytes = rqstp->rq_res.len;
 
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
 
	p = &rdma_resp->rm_xid;
	*p++ = rdma_argp->rm_xid;
	*p++ = rdma_argp->rm_vers;
 	*p++ = rdma->sc_fc_credits;
	*p++ = rp_ary ? rdma_nomsg : rdma_msg;
 
 	/* Start with empty chunks */
 	*p++ = xdr_zero;
 	*p++ = xdr_zero;
 	*p   = xdr_zero;
 
	/* Send any write-chunk data and build resp write-list */
	if (wr_ary) {
		ret = send_write_chunks(rdma, wr_ary, rdma_resp, rqstp, vec);
 		if (ret < 0)
			goto err1;
		inline_bytes -= ret + xdr_padsize(ret);
 	}
	/* Send any reply-list data and update resp reply-list */
	if (rp_ary) {
		ret = send_reply_chunks(rdma, rp_ary, rdma_resp, rqstp, vec);
 		if (ret < 0)
			goto err1;
		inline_bytes -= ret;
 	}
 
	/* Post a fresh Receive buffer _before_ sending the reply */
 	ret = svc_rdma_post_recv(rdma, GFP_KERNEL);
 	if (ret)
 		goto err1;
	ret = send_reply(rdma, rqstp, res_page, rdma_resp, vec,
			 inline_bytes, inv_rkey);
 	if (ret < 0)
 		goto err0;
 
	svc_rdma_put_req_map(rdma, vec);
	dprintk("svcrdma: send_reply returns %d\n", ret);
	return ret;
 
  err1:
 	put_page(res_page);
  err0:
	svc_rdma_put_req_map(rdma, vec);
 	pr_err("svcrdma: Could not send reply, err=%d. Closing transport.\n",
 	       ret);
	set_bit(XPT_CLOSE, &rdma->sc_xprt.xpt_flags);
 	return -ENOTCONN;
 }
