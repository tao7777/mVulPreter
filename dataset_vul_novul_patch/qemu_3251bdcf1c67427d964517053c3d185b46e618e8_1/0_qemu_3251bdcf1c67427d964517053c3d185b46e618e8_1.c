static int bmdma_prepare_buf(IDEDMA *dma, int is_write)
/**
 * Return the number of bytes successfully prepared.
 * -1 on error.
 */
static int32_t bmdma_prepare_buf(IDEDMA *dma, int is_write)
 {
     BMDMAState *bm = DO_UPCAST(BMDMAState, dma, dma);
     IDEState *s = bmdma_active_if(bm);
        uint32_t size;
    } prd;
