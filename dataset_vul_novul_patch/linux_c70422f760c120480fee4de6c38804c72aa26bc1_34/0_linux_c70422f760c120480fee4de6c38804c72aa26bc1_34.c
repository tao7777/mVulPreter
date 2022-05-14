static int send_write(struct svcxprt_rdma *xprt, struct svc_rqst *rqstp,
static int svc_rdma_dma_map_page(struct svcxprt_rdma *rdma,
				 struct svc_rdma_op_ctxt *ctxt,
				 unsigned int sge_no,
				 struct page *page,
				 unsigned int offset,
				 unsigned int len)
 {
	struct ib_device *dev = rdma->sc_cm_id->device;
	dma_addr_t dma_addr;
 
	dma_addr = ib_dma_map_page(dev, page, offset, len, DMA_TO_DEVICE);
	if (ib_dma_mapping_error(dev, dma_addr))
 		return -EIO;
 
	ctxt->sge[sge_no].addr = dma_addr;
	ctxt->sge[sge_no].length = len;
	ctxt->sge[sge_no].lkey = rdma->sc_pd->local_dma_lkey;
	svc_rdma_count_mappings(rdma, ctxt);
	return 0;
}
 
/**
 * svc_rdma_map_reply_hdr - DMA map the transport header buffer
 * @rdma: controlling transport
 * @ctxt: op_ctxt for the Send WR
 * @rdma_resp: buffer containing transport header
 * @len: length of transport header
 *
 * Returns:
 *	%0 if the header is DMA mapped,
 *	%-EIO if DMA mapping failed.
 */
int svc_rdma_map_reply_hdr(struct svcxprt_rdma *rdma,
			   struct svc_rdma_op_ctxt *ctxt,
			   __be32 *rdma_resp,
			   unsigned int len)
{
 	ctxt->direction = DMA_TO_DEVICE;
	ctxt->pages[0] = virt_to_page(rdma_resp);
	ctxt->count = 1;
	return svc_rdma_dma_map_page(rdma, ctxt, 0, ctxt->pages[0], 0, len);
 }
