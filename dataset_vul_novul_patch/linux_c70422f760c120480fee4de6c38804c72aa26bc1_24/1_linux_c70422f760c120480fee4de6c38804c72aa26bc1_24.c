void svc_rdma_xdr_encode_array_chunk(struct rpcrdma_write_array *ary,
				     int chunk_no,
				     __be32 rs_handle,
				     __be64 rs_offset,
				     u32 write_len)
{
	struct rpcrdma_segment *seg = &ary->wc_array[chunk_no].wc_target;
	seg->rs_handle = rs_handle;
	seg->rs_offset = rs_offset;
	seg->rs_length = cpu_to_be32(write_len);
}
