int svc_rdma_xdr_encode_error(struct svcxprt_rdma *xprt,
			      struct rpcrdma_msg *rmsgp,
			      enum rpcrdma_errcode err, __be32 *va)
{
	__be32 *startp = va;
	*va++ = rmsgp->rm_xid;
	*va++ = rmsgp->rm_vers;
	*va++ = xprt->sc_fc_credits;
	*va++ = rdma_error;
	*va++ = cpu_to_be32(err);
	if (err == ERR_VERS) {
		*va++ = rpcrdma_version;
		*va++ = rpcrdma_version;
	}
	return (int)((unsigned long)va - (unsigned long)startp);
}
