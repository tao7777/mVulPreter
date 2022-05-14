static int send_write_chunks(struct svcxprt_rdma *xprt,
/* Load the xdr_buf into the ctxt's sge array, and DMA map each
 * element as it is added.
 *
 * Returns the number of sge elements loaded on success, or
 * a negative errno on failure.
 */
static int svc_rdma_map_reply_msg(struct svcxprt_rdma *rdma,
				  struct svc_rdma_op_ctxt *ctxt,
				  struct xdr_buf *xdr, __be32 *wr_lst)
 {
	unsigned int len, sge_no, remaining, page_off;
	struct page **ppages;
	unsigned char *base;
	u32 xdr_pad;
 	int ret;
 
	sge_no = 1;

	ret = svc_rdma_dma_map_buf(rdma, ctxt, sge_no++,
				   xdr->head[0].iov_base,
				   xdr->head[0].iov_len);
	if (ret < 0)
		return ret;

	/* If a Write chunk is present, the xdr_buf's page list
	 * is not included inline. However the Upper Layer may
	 * have added XDR padding in the tail buffer, and that
	 * should not be included inline.
	 */
	if (wr_lst) {
		base = xdr->tail[0].iov_base;
		len = xdr->tail[0].iov_len;
		xdr_pad = xdr_padsize(xdr->page_len);

		if (len && xdr_pad) {
			base += xdr_pad;
			len -= xdr_pad;
 		}

		goto tail;
	}

	ppages = xdr->pages + (xdr->page_base >> PAGE_SHIFT);
	page_off = xdr->page_base & ~PAGE_MASK;
	remaining = xdr->page_len;
	while (remaining) {
		len = min_t(u32, PAGE_SIZE - page_off, remaining);

		ret = svc_rdma_dma_map_page(rdma, ctxt, sge_no++,
					    *ppages++, page_off, len);
		if (ret < 0)
			return ret;

		remaining -= len;
		page_off = 0;
 	}
 
	base = xdr->tail[0].iov_base;
	len = xdr->tail[0].iov_len;
tail:
	if (len) {
		ret = svc_rdma_dma_map_buf(rdma, ctxt, sge_no++, base, len);
		if (ret < 0)
			return ret;
	}
 
	return sge_no - 1;
 }
