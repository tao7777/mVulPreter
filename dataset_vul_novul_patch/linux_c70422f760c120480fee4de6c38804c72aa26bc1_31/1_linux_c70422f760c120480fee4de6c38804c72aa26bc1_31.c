static dma_addr_t dma_map_xdr(struct svcxprt_rdma *xprt,
			      struct xdr_buf *xdr,
			      u32 xdr_off, size_t len, int dir)
 {
	struct page *page;
	dma_addr_t dma_addr;
	if (xdr_off < xdr->head[0].iov_len) {
		/* This offset is in the head */
		xdr_off += (unsigned long)xdr->head[0].iov_base & ~PAGE_MASK;
		page = virt_to_page(xdr->head[0].iov_base);
	} else {
		xdr_off -= xdr->head[0].iov_len;
		if (xdr_off < xdr->page_len) {
			/* This offset is in the page list */
			xdr_off += xdr->page_base;
			page = xdr->pages[xdr_off >> PAGE_SHIFT];
			xdr_off &= ~PAGE_MASK;
		} else {
			/* This offset is in the tail */
			xdr_off -= xdr->page_len;
			xdr_off += (unsigned long)
				xdr->tail[0].iov_base & ~PAGE_MASK;
			page = virt_to_page(xdr->tail[0].iov_base);
		}
 	}
	dma_addr = ib_dma_map_page(xprt->sc_cm_id->device, page, xdr_off,
				   min_t(size_t, PAGE_SIZE, len), dir);
	return dma_addr;
 }
