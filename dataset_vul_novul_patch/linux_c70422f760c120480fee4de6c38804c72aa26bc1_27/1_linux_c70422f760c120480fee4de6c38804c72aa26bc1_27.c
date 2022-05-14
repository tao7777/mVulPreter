void svc_rdma_xdr_encode_write_list(struct rpcrdma_msg *rmsgp, int chunks)
{
	struct rpcrdma_write_array *ary;
	/* no read-list */
	rmsgp->rm_body.rm_chunks[0] = xdr_zero;
	/* write-array discrim */
	ary = (struct rpcrdma_write_array *)
		&rmsgp->rm_body.rm_chunks[1];
	ary->wc_discrim = xdr_one;
	ary->wc_nchunks = cpu_to_be32(chunks);
	/* write-list terminator */
	ary->wc_array[chunks].wc_target.rs_handle = xdr_zero;
	/* reply-array discriminator */
	ary->wc_array[chunks].wc_target.rs_length = xdr_zero;
}
