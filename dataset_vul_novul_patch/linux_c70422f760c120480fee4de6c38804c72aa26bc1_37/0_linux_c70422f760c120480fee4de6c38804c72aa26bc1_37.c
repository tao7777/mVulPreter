static void svc_rdma_get_write_arrays(struct rpcrdma_msg *rmsgp,
static void svc_rdma_get_write_arrays(__be32 *rdma_argp,
				      __be32 **write, __be32 **reply)
 {
 	__be32 *p;
 
	p = rdma_argp + rpcrdma_fixed_maxsz;
 
 	/* Read list */
 	while (*p++ != xdr_zero)
 		p += 5;
 
 	/* Write list */
 	if (*p != xdr_zero) {
		*write = p;
 		while (*p++ != xdr_zero)
 			p += 1 + be32_to_cpu(*p) * 4;
 	} else {
		*write = NULL;
		p++;
	}
 
 	/* Reply chunk */
 	if (*p != xdr_zero)
		*reply = p;
 	else
 		*reply = NULL;
 }
