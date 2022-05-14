svc_rdma_is_backchannel_reply(struct svc_xprt *xprt, struct rpcrdma_msg *rmsgp)
static bool svc_rdma_is_backchannel_reply(struct svc_xprt *xprt,
					  __be32 *rdma_resp)
 {
	__be32 *p;
 
 	if (!xprt->xpt_bc_xprt)
 		return false;
 
	p = rdma_resp + 3;
	if (*p++ != rdma_msg)
 		return false;

	if (*p++ != xdr_zero)
 		return false;
	if (*p++ != xdr_zero)
 		return false;
	if (*p++ != xdr_zero)
 		return false;
 
	/* XID sanity */
	if (*p++ != *rdma_resp)
 		return false;
 	/* call direction */
	if (*p == cpu_to_be32(RPC_CALL))
 		return false;
 
 	return true;
}
