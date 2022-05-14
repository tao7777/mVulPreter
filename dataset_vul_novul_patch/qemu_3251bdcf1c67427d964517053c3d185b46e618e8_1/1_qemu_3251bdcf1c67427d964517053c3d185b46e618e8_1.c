static int bmdma_prepare_buf(IDEDMA *dma, int is_write)
 {
     BMDMAState *bm = DO_UPCAST(BMDMAState, dma, dma);
     IDEState *s = bmdma_active_if(bm);
        uint32_t size;
    } prd;
