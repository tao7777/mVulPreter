u32 h264bsdInitDpb(
 dpbStorage_t *dpb,
  u32 picSizeInMbs,
  u32 dpbSize,
  u32 maxRefFrames,
  u32 maxFrameNum,
  u32 noReordering)
{

/* Variables */

    u32 i;

/* Code */

    ASSERT(picSizeInMbs);
    ASSERT(maxRefFrames <= MAX_NUM_REF_PICS);
    ASSERT(maxRefFrames <= dpbSize);

     ASSERT(maxFrameNum);
     ASSERT(dpbSize);
 
    // see comment in loop below about size calculation
    if (picSizeInMbs > (UINT32_MAX - 32 - 15) / 384) {
        ALOGE("b/28533562");
        android_errorWriteLog(0x534e4554, "28533562");
        return(MEMORY_ALLOCATION_ERROR);
    }

     dpb->maxLongTermFrameIdx = NO_LONG_TERM_FRAME_INDICES;
     dpb->maxRefFrames        = MAX(maxRefFrames, 1);
     if (noReordering)
        dpb->dpbSize         = dpb->maxRefFrames;
 else
        dpb->dpbSize         = dpbSize;
    dpb->maxFrameNum         = maxFrameNum;
    dpb->noReordering        = noReordering;
    dpb->fullness            = 0;
    dpb->numRefFrames        = 0;
    dpb->prevRefFrameNum     = 0;

    ALLOCATE(dpb->buffer, MAX_NUM_REF_IDX_L0_ACTIVE + 1, dpbPicture_t);
 if (dpb->buffer == NULL)
 return(MEMORY_ALLOCATION_ERROR);
    H264SwDecMemset(dpb->buffer, 0,
 (MAX_NUM_REF_IDX_L0_ACTIVE + 1)*sizeof(dpbPicture_t));
 for (i = 0; i < dpb->dpbSize + 1; i++)
 {
 /* Allocate needed amount of memory, which is:
         * image size + 32 + 15, where 32 cames from the fact that in ARM OpenMax
         * DL implementation Functions may read beyond the end of an array,
         * by a maximum of 32 bytes. And +15 cames for the need to align memory
         * to 16-byte boundary */
        ALLOCATE(dpb->buffer[i].pAllocatedData, (picSizeInMbs*384 + 32+15), u8);
 if (dpb->buffer[i].pAllocatedData == NULL)
 return(MEMORY_ALLOCATION_ERROR);

        dpb->buffer[i].data = ALIGN(dpb->buffer[i].pAllocatedData, 16);
 }

    ALLOCATE(dpb->list, MAX_NUM_REF_IDX_L0_ACTIVE + 1, dpbPicture_t*);
    ALLOCATE(dpb->outBuf, dpb->dpbSize+1, dpbOutPicture_t);

 if (dpb->list == NULL || dpb->outBuf == NULL)
 return(MEMORY_ALLOCATION_ERROR);

    H264SwDecMemset(dpb->list, 0,
 ((MAX_NUM_REF_IDX_L0_ACTIVE + 1) * sizeof(dpbPicture_t*)) );

    dpb->numOut = dpb->outIndex = 0;

 return(HANTRO_OK);

}
