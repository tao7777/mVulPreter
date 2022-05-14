int svc_rdma_handle_bc_reply(struct rpc_xprt *xprt, struct rpcrdma_msg *rmsgp,
 			     struct xdr_buf *rcvbuf)
 {
 	struct rpcrdma_xprt *r_xprt = rpcx_to_rdmax(xprt);
	struct kvec *dst, *src = &rcvbuf->head[0];
	struct rpc_rqst *req;
	unsigned long cwnd;
	u32 credits;
	size_t len;
	__be32 xid;
	__be32 *p;
	int ret;
 
 	p = (__be32 *)src->iov_base;
 	len = src->iov_len;
	xid = rmsgp->rm_xid;
 
 #ifdef SVCRDMA_BACKCHANNEL_DEBUG
 	pr_info("%s: xid=%08x, length=%zu\n",
 		__func__, be32_to_cpu(xid), len);
 	pr_info("%s: RPC/RDMA: %*ph\n",
		__func__, (int)RPCRDMA_HDRLEN_MIN, rmsgp);
 	pr_info("%s:      RPC: %*ph\n",
 		__func__, (int)len, p);
 #endif

	ret = -EAGAIN;
	if (src->iov_len < 24)
		goto out_shortreply;

	spin_lock_bh(&xprt->transport_lock);
	req = xprt_lookup_rqst(xprt, xid);
	if (!req)
		goto out_notfound;

	dst = &req->rq_private_buf.head[0];
	memcpy(&req->rq_private_buf, &req->rq_rcv_buf, sizeof(struct xdr_buf));
	if (dst->iov_len < len)
 		goto out_unlock;
 	memcpy(dst->iov_base, p, len);
 
	credits = be32_to_cpu(rmsgp->rm_credit);
 	if (credits == 0)
 		credits = 1;	/* don't deadlock */
 	else if (credits > r_xprt->rx_buf.rb_bc_max_requests)
		credits = r_xprt->rx_buf.rb_bc_max_requests;

	cwnd = xprt->cwnd;
	xprt->cwnd = credits << RPC_CWNDSHIFT;
	if (xprt->cwnd > cwnd)
		xprt_release_rqst_cong(req->rq_task);

	ret = 0;
	xprt_complete_rqst(req->rq_task, rcvbuf->len);
	rcvbuf->len = 0;

out_unlock:
	spin_unlock_bh(&xprt->transport_lock);
out:
	return ret;

out_shortreply:
	dprintk("svcrdma: short bc reply: xprt=%p, len=%zu\n",
		xprt, src->iov_len);
	goto out;

out_notfound:
	dprintk("svcrdma: unrecognized bc reply: xprt=%p, xid=%08x\n",
		xprt, be32_to_cpu(xid));

	goto out_unlock;
}
