unsigned int svc_rdma_xdr_get_reply_hdr_len(__be32 *rdma_resp)
{
	unsigned int nsegs;
	__be32 *p;
	p = rdma_resp;
	/* RPC-over-RDMA V1 replies never have a Read list. */
	p += rpcrdma_fixed_maxsz + 1;
	/* Skip Write list. */
	while (*p++ != xdr_zero) {
		nsegs = be32_to_cpup(p++);
		p += nsegs * rpcrdma_segment_maxsz;
	}
	/* Skip Reply chunk. */
	if (*p++ != xdr_zero) {
		nsegs = be32_to_cpup(p++);
		p += nsegs * rpcrdma_segment_maxsz;
	}
	return (unsigned long)p - (unsigned long)rdma_resp;
}
