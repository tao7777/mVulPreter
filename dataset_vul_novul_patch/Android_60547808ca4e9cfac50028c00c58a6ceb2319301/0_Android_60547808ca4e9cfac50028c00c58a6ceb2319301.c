u32 h264bsdActivateParamSets(storage_t *pStorage, u32 ppsId, u32 isIdr)
{

/* Variables */

    u32 tmp;
    u32 flag;

/* Code */

    ASSERT(pStorage);
    ASSERT(ppsId < MAX_NUM_PIC_PARAM_SETS);

 /* check that pps and corresponding sps exist */
 if ( (pStorage->pps[ppsId] == NULL) ||
 (pStorage->sps[pStorage->pps[ppsId]->seqParameterSetId] == NULL) )
 {
 return(HANTRO_NOK);
 }

 /* check that pps parameters do not violate picture size constraints */
    tmp = CheckPps(pStorage->pps[ppsId],
                   pStorage->sps[pStorage->pps[ppsId]->seqParameterSetId]);
 if (tmp != HANTRO_OK)
 return(tmp);

 /* first activation part1 */
 if (pStorage->activePpsId == MAX_NUM_PIC_PARAM_SETS)
 {
        pStorage->activePpsId = ppsId;

         pStorage->activePps = pStorage->pps[ppsId];
         pStorage->activeSpsId = pStorage->activePps->seqParameterSetId;
         pStorage->activeSps = pStorage->sps[pStorage->activeSpsId];

        /* report error before multiplication to prevent integer overflow */
        if (pStorage->activeSps->picWidthInMbs == 0)
        {
            pStorage->picSizeInMbs = 0;
        }
        else if (pStorage->activeSps->picHeightInMbs >
                 UINT32_MAX / pStorage->activeSps->picWidthInMbs)
        {
            return(MEMORY_ALLOCATION_ERROR);
        }
        else
        {
            pStorage->picSizeInMbs =
                pStorage->activeSps->picWidthInMbs *
                pStorage->activeSps->picHeightInMbs;
        }
 
         pStorage->currImage->width = pStorage->activeSps->picWidthInMbs;
         pStorage->currImage->height = pStorage->activeSps->picHeightInMbs;

        pStorage->pendingActivation = HANTRO_TRUE;
 }
 /* first activation part2 */
 else if (pStorage->pendingActivation)
 {
        pStorage->pendingActivation = HANTRO_FALSE;

        FREE(pStorage->mb);
        FREE(pStorage->sliceGroupMap);

        ALLOCATE(pStorage->mb, pStorage->picSizeInMbs, mbStorage_t);
        ALLOCATE(pStorage->sliceGroupMap, pStorage->picSizeInMbs, u32);
 if (pStorage->mb == NULL || pStorage->sliceGroupMap == NULL)
 return(MEMORY_ALLOCATION_ERROR);

        H264SwDecMemset(pStorage->mb, 0,
            pStorage->picSizeInMbs * sizeof(mbStorage_t));

        h264bsdInitMbNeighbours(pStorage->mb,
            pStorage->activeSps->picWidthInMbs,
            pStorage->picSizeInMbs);

 /* dpb output reordering disabled if
         * 1) application set noReordering flag
         * 2) POC type equal to 2
         * 3) num_reorder_frames in vui equal to 0 */
 if ( pStorage->noReordering ||
             pStorage->activeSps->picOrderCntType == 2 ||
 (pStorage->activeSps->vuiParametersPresentFlag &&
              pStorage->activeSps->vuiParameters->bitstreamRestrictionFlag &&
 !pStorage->activeSps->vuiParameters->numReorderFrames) )
            flag = HANTRO_TRUE;
 else
            flag = HANTRO_FALSE;

        tmp = h264bsdResetDpb(pStorage->dpb,
            pStorage->activeSps->picWidthInMbs *
            pStorage->activeSps->picHeightInMbs,
            pStorage->activeSps->maxDpbSize,
            pStorage->activeSps->numRefFrames,
            pStorage->activeSps->maxFrameNum,
            flag);
 if (tmp != HANTRO_OK)
 return(tmp);
 }
 else if (ppsId != pStorage->activePpsId)
 {
 /* sequence parameter set shall not change but before an IDR picture */
 if (pStorage->pps[ppsId]->seqParameterSetId != pStorage->activeSpsId)
 {
            DEBUG(("SEQ PARAM SET CHANGING...\n"));
 if (isIdr)
 {
                pStorage->activePpsId = ppsId;
                pStorage->activePps = pStorage->pps[ppsId];
                pStorage->activeSpsId = pStorage->activePps->seqParameterSetId;
                pStorage->activeSps = pStorage->sps[pStorage->activeSpsId];
                pStorage->picSizeInMbs =
                    pStorage->activeSps->picWidthInMbs *
                    pStorage->activeSps->picHeightInMbs;

                pStorage->currImage->width = pStorage->activeSps->picWidthInMbs;
                pStorage->currImage->height =
                    pStorage->activeSps->picHeightInMbs;

                pStorage->pendingActivation = HANTRO_TRUE;
 }
 else
 {
                DEBUG(("TRYING TO CHANGE SPS IN NON-IDR SLICE\n"));
 return(HANTRO_NOK);
 }
 }
 else
 {
            pStorage->activePpsId = ppsId;
            pStorage->activePps = pStorage->pps[ppsId];
 }
 }

 return(HANTRO_OK);

}
