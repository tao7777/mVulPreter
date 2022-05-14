static int send_write(struct svcxprt_rdma *xprt, struct svc_rqst *rqstp,
		      u32 rmr, u64 to,
		      u32 xdr_off, int write_len,
		      struct svc_rdma_req_map *vec)
 {
	struct ib_rdma_wr write_wr;
	struct ib_sge *sge;
	int xdr_sge_no;
	int sge_no;
	int sge_bytes;
	int sge_off;
	int bc;
	struct svc_rdma_op_ctxt *ctxt;
 
	if (vec->count > RPCSVC_MAXPAGES) {
		pr_err("svcrdma: Too many pages (%lu)\n", vec->count);
 		return -EIO;
	}
 
	dprintk("svcrdma: RDMA_WRITE rmr=%x, to=%llx, xdr_off=%d, "
		"write_len=%d, vec->sge=%p, vec->count=%lu\n",
		rmr, (unsigned long long)to, xdr_off,
		write_len, vec->sge, vec->count);
 
	ctxt = svc_rdma_get_context(xprt);
 	ctxt->direction = DMA_TO_DEVICE;
	sge = ctxt->sge;
	/* Find the SGE associated with xdr_off */
	for (bc = xdr_off, xdr_sge_no = 1; bc && xdr_sge_no < vec->count;
	     xdr_sge_no++) {
		if (vec->sge[xdr_sge_no].iov_len > bc)
			break;
		bc -= vec->sge[xdr_sge_no].iov_len;
	}
	sge_off = bc;
	bc = write_len;
	sge_no = 0;
	/* Copy the remaining SGE */
	while (bc != 0) {
		sge_bytes = min_t(size_t,
			  bc, vec->sge[xdr_sge_no].iov_len-sge_off);
		sge[sge_no].length = sge_bytes;
		sge[sge_no].addr =
			dma_map_xdr(xprt, &rqstp->rq_res, xdr_off,
				    sge_bytes, DMA_TO_DEVICE);
		xdr_off += sge_bytes;
		if (ib_dma_mapping_error(xprt->sc_cm_id->device,
					 sge[sge_no].addr))
			goto err;
		svc_rdma_count_mappings(xprt, ctxt);
		sge[sge_no].lkey = xprt->sc_pd->local_dma_lkey;
		ctxt->count++;
		sge_off = 0;
		sge_no++;
		xdr_sge_no++;
		if (xdr_sge_no > vec->count) {
			pr_err("svcrdma: Too many sges (%d)\n", xdr_sge_no);
			goto err;
		}
		bc -= sge_bytes;
		if (sge_no == xprt->sc_max_sge)
			break;
	}
	/* Prepare WRITE WR */
	memset(&write_wr, 0, sizeof write_wr);
	ctxt->cqe.done = svc_rdma_wc_write;
	write_wr.wr.wr_cqe = &ctxt->cqe;
	write_wr.wr.sg_list = &sge[0];
	write_wr.wr.num_sge = sge_no;
	write_wr.wr.opcode = IB_WR_RDMA_WRITE;
	write_wr.wr.send_flags = IB_SEND_SIGNALED;
	write_wr.rkey = rmr;
	write_wr.remote_addr = to;
	/* Post It */
	atomic_inc(&rdma_stat_write);
	if (svc_rdma_send(xprt, &write_wr.wr))
		goto err;
	return write_len - bc;
 err:
	svc_rdma_unmap_dma(ctxt);
	svc_rdma_put_context(ctxt, 0);
	return -EIO;
 }
