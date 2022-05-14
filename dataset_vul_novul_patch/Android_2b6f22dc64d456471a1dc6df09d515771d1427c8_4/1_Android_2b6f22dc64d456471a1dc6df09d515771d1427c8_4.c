u32 h264bsdInit(storage_t *pStorage, u32 noOutputReordering)
{

/* Variables */
    u32 size;
/* Code */

    ASSERT(pStorage);

    h264bsdInitStorage(pStorage);

 /* allocate mbLayer to be next multiple of 64 to enable use of

      * specific NEON optimized "memset" for clearing the structure */
     size = (sizeof(macroblockLayer_t) + 63) & ~0x3F;
 
    pStorage->mbLayer = (macroblockLayer_t*)H264SwDecMalloc(size);
     if (!pStorage->mbLayer)
         return HANTRO_NOK;
 
 if (noOutputReordering)
        pStorage->noReordering = HANTRO_TRUE;

 return HANTRO_OK;
}
