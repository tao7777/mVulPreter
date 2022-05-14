int svc_rdma_map_xdr(struct svcxprt_rdma *xprt,
/* Returns length of transport header, in bytes.
 */
static unsigned int svc_rdma_reply_hdr_len(__be32 *rdma_resp)
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
 
/* One Write chunk is copied from Call transport header to Reply
 * transport header. Each segment's length field is updated to
 * reflect number of bytes consumed in the segment.
 *
 * Returns number of segments in this chunk.
 */
static unsigned int xdr_encode_write_chunk(__be32 *dst, __be32 *src,
					   unsigned int remaining)
{
	unsigned int i, nsegs;
	u32 seg_len;

	/* Write list discriminator */
	*dst++ = *src++;

	/* number of segments in this chunk */
	nsegs = be32_to_cpup(src);
	*dst++ = *src++;

	for (i = nsegs; i; i--) {
		/* segment's RDMA handle */
		*dst++ = *src++;

		/* bytes returned in this segment */
		seg_len = be32_to_cpu(*src);
		if (remaining >= seg_len) {
			/* entire segment was consumed */
			*dst = *src;
			remaining -= seg_len;
		} else {
			/* segment only partly filled */
			*dst = cpu_to_be32(remaining);
			remaining = 0;
 		}
		dst++; src++;
 
		/* segment's RDMA offset */
		*dst++ = *src++;
		*dst++ = *src++;
	}
 
	return nsegs;
 }
