static int send_write_chunks(struct svcxprt_rdma *xprt,
			     struct rpcrdma_write_array *wr_ary,
			     struct rpcrdma_msg *rdma_resp,
			     struct svc_rqst *rqstp,
			     struct svc_rdma_req_map *vec)
 {
	u32 xfer_len = rqstp->rq_res.page_len;
	int write_len;
	u32 xdr_off;
	int chunk_off;
	int chunk_no;
	int nchunks;
	struct rpcrdma_write_array *res_ary;
 	int ret;
 
	res_ary = (struct rpcrdma_write_array *)
		&rdma_resp->rm_body.rm_chunks[1];
	/* Write chunks start at the pagelist */
	nchunks = be32_to_cpu(wr_ary->wc_nchunks);
	for (xdr_off = rqstp->rq_res.head[0].iov_len, chunk_no = 0;
	     xfer_len && chunk_no < nchunks;
	     chunk_no++) {
		struct rpcrdma_segment *arg_ch;
		u64 rs_offset;
		arg_ch = &wr_ary->wc_array[chunk_no].wc_target;
		write_len = min(xfer_len, be32_to_cpu(arg_ch->rs_length));
		/* Prepare the response chunk given the length actually
		 * written */
		xdr_decode_hyper((__be32 *)&arg_ch->rs_offset, &rs_offset);
		svc_rdma_xdr_encode_array_chunk(res_ary, chunk_no,
						arg_ch->rs_handle,
						arg_ch->rs_offset,
						write_len);
		chunk_off = 0;
		while (write_len) {
			ret = send_write(xprt, rqstp,
					 be32_to_cpu(arg_ch->rs_handle),
					 rs_offset + chunk_off,
					 xdr_off,
					 write_len,
					 vec);
			if (ret <= 0)
				goto out_err;
			chunk_off += ret;
			xdr_off += ret;
			xfer_len -= ret;
			write_len -= ret;
 		}
 	}
	/* Update the req with the number of chunks actually used */
	svc_rdma_xdr_encode_write_list(rdma_resp, chunk_no);
 
	return rqstp->rq_res.page_len;
 
out_err:
	pr_err("svcrdma: failed to send write chunks, rc=%d\n", ret);
	return -EIO;
 }
