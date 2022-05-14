static u32 svc_rdma_get_inv_rkey(struct rpcrdma_msg *rdma_argp,
				 struct rpcrdma_write_array *wr_ary,
				 struct rpcrdma_write_array *rp_ary)
 {
	struct rpcrdma_read_chunk *rd_ary;
	struct rpcrdma_segment *arg_ch;
 
	rd_ary = (struct rpcrdma_read_chunk *)&rdma_argp->rm_body.rm_chunks[0];
	if (rd_ary->rc_discrim != xdr_zero)
		return be32_to_cpu(rd_ary->rc_target.rs_handle);
 
	if (wr_ary && be32_to_cpu(wr_ary->wc_nchunks)) {
		arg_ch = &wr_ary->wc_array[0].wc_target;
		return be32_to_cpu(arg_ch->rs_handle);
	}
 
	if (rp_ary && be32_to_cpu(rp_ary->wc_nchunks)) {
		arg_ch = &rp_ary->wc_array[0].wc_target;
		return be32_to_cpu(arg_ch->rs_handle);
	}
 
 	return 0;
 }
