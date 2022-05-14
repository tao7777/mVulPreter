void svc_rdma_xdr_encode_reply_array(struct rpcrdma_write_array *ary,
				 int chunks)
{
	ary->wc_discrim = xdr_one;
	ary->wc_nchunks = cpu_to_be32(chunks);
}
