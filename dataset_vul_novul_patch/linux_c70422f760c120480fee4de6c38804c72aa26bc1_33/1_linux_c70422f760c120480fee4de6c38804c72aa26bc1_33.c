static int send_reply_chunks(struct svcxprt_rdma *xprt,
			     struct rpcrdma_write_array *rp_ary,
			     struct rpcrdma_msg *rdma_resp,
			     struct svc_rqst *rqstp,
			     struct svc_rdma_req_map *vec)
 {
	u32 xfer_len = rqstp->rq_res.len;
	int write_len;
	u32 xdr_off;
	int chunk_no;
	int chunk_off;
	int nchunks;
	struct rpcrdma_segment *ch;
	struct rpcrdma_write_array *res_ary;
	int ret;
 
	/* XXX: need to fix when reply lists occur with read-list and or
	 * write-list */
	res_ary = (struct rpcrdma_write_array *)
		&rdma_resp->rm_body.rm_chunks[2];
	/* xdr offset starts at RPC message */
	nchunks = be32_to_cpu(rp_ary->wc_nchunks);
	for (xdr_off = 0, chunk_no = 0;
	     xfer_len && chunk_no < nchunks;
	     chunk_no++) {
		u64 rs_offset;
		ch = &rp_ary->wc_array[chunk_no].wc_target;
		write_len = min(xfer_len, be32_to_cpu(ch->rs_length));
		/* Prepare the reply chunk given the length actually
		 * written */
		xdr_decode_hyper((__be32 *)&ch->rs_offset, &rs_offset);
		svc_rdma_xdr_encode_array_chunk(res_ary, chunk_no,
						ch->rs_handle, ch->rs_offset,
						write_len);
		chunk_off = 0;
		while (write_len) {
			ret = send_write(xprt, rqstp,
					 be32_to_cpu(ch->rs_handle),
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
	svc_rdma_xdr_encode_reply_array(res_ary, chunk_no);
 
	return rqstp->rq_res.len;
 
out_err:
	pr_err("svcrdma: failed to send reply chunks, rc=%d\n", ret);
	return -EIO;
 }
