static void svc_rdma_get_write_arrays(struct rpcrdma_msg *rmsgp,
				      struct rpcrdma_write_array **write,
				      struct rpcrdma_write_array **reply)
 {
 	__be32 *p;
 
	p = (__be32 *)&rmsgp->rm_body.rm_chunks[0];
 
 	/* Read list */
 	while (*p++ != xdr_zero)
 		p += 5;
 
 	/* Write list */
 	if (*p != xdr_zero) {
		*write = (struct rpcrdma_write_array *)p;
 		while (*p++ != xdr_zero)
 			p += 1 + be32_to_cpu(*p) * 4;
 	} else {
		*write = NULL;
		p++;
	}
 
 	/* Reply chunk */
 	if (*p != xdr_zero)
		*reply = (struct rpcrdma_write_array *)p;
 	else
 		*reply = NULL;
 }
