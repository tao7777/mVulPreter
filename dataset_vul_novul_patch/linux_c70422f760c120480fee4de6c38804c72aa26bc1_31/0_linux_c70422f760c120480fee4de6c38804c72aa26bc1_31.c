static dma_addr_t dma_map_xdr(struct svcxprt_rdma *xprt,
/* The client provided a Write list in the Call message. Fill in
 * the segments in the first Write chunk in the Reply's transport
 * header with the number of bytes consumed in each segment.
 * Remaining chunks are returned unused.
 *
 * Assumptions:
 *  - Client has provided only one Write chunk
 */
static void svc_rdma_xdr_encode_write_list(__be32 *rdma_resp, __be32 *wr_ch,
					   unsigned int consumed)
 {
	unsigned int nsegs;
	__be32 *p, *q;

	/* RPC-over-RDMA V1 replies never have a Read list. */
	p = rdma_resp + rpcrdma_fixed_maxsz + 1;

	q = wr_ch;
	while (*q != xdr_zero) {
		nsegs = xdr_encode_write_chunk(p, q, consumed);
		q += 2 + nsegs * rpcrdma_segment_maxsz;
		p += 2 + nsegs * rpcrdma_segment_maxsz;
		consumed = 0;
 	}

	/* Terminate Write list */
	*p++ = xdr_zero;

	/* Reply chunk discriminator; may be replaced later */
	*p = xdr_zero;
}

/* The client provided a Reply chunk in the Call message. Fill in
 * the segments in the Reply chunk in the Reply message with the
 * number of bytes consumed in each segment.
 *
 * Assumptions:
 * - Reply can always fit in the provided Reply chunk
 */
static void svc_rdma_xdr_encode_reply_chunk(__be32 *rdma_resp, __be32 *rp_ch,
					    unsigned int consumed)
{
	__be32 *p;

	/* Find the Reply chunk in the Reply's xprt header.
	 * RPC-over-RDMA V1 replies never have a Read list.
	 */
	p = rdma_resp + rpcrdma_fixed_maxsz + 1;

	/* Skip past Write list */
	while (*p++ != xdr_zero)
		p += 1 + be32_to_cpup(p) * rpcrdma_segment_maxsz;

	xdr_encode_write_chunk(p, rp_ch, consumed);
 }
