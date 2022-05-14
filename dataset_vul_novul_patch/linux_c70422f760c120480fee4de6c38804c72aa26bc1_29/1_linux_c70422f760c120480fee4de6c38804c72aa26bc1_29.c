svc_rdma_is_backchannel_reply(struct svc_xprt *xprt, struct rpcrdma_msg *rmsgp)
 {
	__be32 *p = (__be32 *)rmsgp;
 
 	if (!xprt->xpt_bc_xprt)
 		return false;
 
	if (rmsgp->rm_type != rdma_msg)
 		return false;
	if (rmsgp->rm_body.rm_chunks[0] != xdr_zero)
 		return false;
	if (rmsgp->rm_body.rm_chunks[1] != xdr_zero)
 		return false;
	if (rmsgp->rm_body.rm_chunks[2] != xdr_zero)
 		return false;
 
	/* sanity */
	if (p[7] != rmsgp->rm_xid)
 		return false;
 	/* call direction */
	if (p[8] == cpu_to_be32(RPC_CALL))
 		return false;
 
 	return true;
}
