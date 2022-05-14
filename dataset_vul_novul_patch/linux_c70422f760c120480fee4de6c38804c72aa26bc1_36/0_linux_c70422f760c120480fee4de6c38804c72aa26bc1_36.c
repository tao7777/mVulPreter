static u32 svc_rdma_get_inv_rkey(struct rpcrdma_msg *rdma_argp,
static u32 svc_rdma_get_inv_rkey(__be32 *rdma_argp,
				 __be32 *wr_lst, __be32 *rp_ch)
 {
	__be32 *p;
 
	p = rdma_argp + rpcrdma_fixed_maxsz;
	if (*p != xdr_zero)
		p += 2;
	else if (wr_lst && be32_to_cpup(wr_lst + 1))
		p = wr_lst + 2;
	else if (rp_ch && be32_to_cpup(rp_ch + 1))
		p = rp_ch + 2;
	else
		return 0;
	return be32_to_cpup(p);
}
 
/* ib_dma_map_page() is used here because svc_rdma_dma_unmap()
 * is used during completion to DMA-unmap this memory, and
 * it uses ib_dma_unmap_page() exclusively.
 */
static int svc_rdma_dma_map_buf(struct svcxprt_rdma *rdma,
				struct svc_rdma_op_ctxt *ctxt,
				unsigned int sge_no,
				unsigned char *base,
				unsigned int len)
{
	unsigned long offset = (unsigned long)base & ~PAGE_MASK;
	struct ib_device *dev = rdma->sc_cm_id->device;
	dma_addr_t dma_addr;
 
	dma_addr = ib_dma_map_page(dev, virt_to_page(base),
				   offset, len, DMA_TO_DEVICE);
	if (ib_dma_mapping_error(dev, dma_addr))
		return -EIO;
 
	ctxt->sge[sge_no].addr = dma_addr;
	ctxt->sge[sge_no].length = len;
	ctxt->sge[sge_no].lkey = rdma->sc_pd->local_dma_lkey;
	svc_rdma_count_mappings(rdma, ctxt);
 	return 0;
 }
