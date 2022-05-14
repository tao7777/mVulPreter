H264SwDecRet H264SwDecInit(H264SwDecInst *decInst, u32 noOutputReordering)
{
    u32 rv = 0;

 decContainer_t *pDecCont;

    DEC_API_TRC("H264SwDecInit#");

 /* check that right shift on negative numbers is performed signed */
 /*lint -save -e* following check causes multiple lint messages */
 if ( ((-1)>>1) != (-1) )
 {
        DEC_API_TRC("H264SwDecInit# ERROR: Right shift is not signed");
 return(H264SWDEC_INITFAIL);
 }
 /*lint -restore */

 if (decInst == NULL)
 {
        DEC_API_TRC("H264SwDecInit# ERROR: decInst == NULL");

         return(H264SWDEC_PARAM_ERR);
     }
 
    pDecCont = (decContainer_t *)H264SwDecMalloc(sizeof(decContainer_t), 1);
 
     if (pDecCont == NULL)
     {
        DEC_API_TRC("H264SwDecInit# ERROR: Memory allocation failed");
 return(H264SWDEC_MEMFAIL);
 }

#ifdef H264DEC_TRACE
    sprintf(pDecCont->str, "H264SwDecInit# decInst %p noOutputReordering %d",
 (void*)decInst, noOutputReordering);
    DEC_API_TRC(pDecCont->str);
#endif

    rv = h264bsdInit(&pDecCont->storage, noOutputReordering);
 if (rv != HANTRO_OK)
 {
        H264SwDecRelease(pDecCont);
 return(H264SWDEC_MEMFAIL);
 }

    pDecCont->decStat  = INITIALIZED;
    pDecCont->picNumber = 0;

#ifdef H264DEC_TRACE
    sprintf(pDecCont->str, "H264SwDecInit# OK: return %p", (void*)pDecCont);
    DEC_API_TRC(pDecCont->str);
#endif

 *decInst = (decContainer_t *)pDecCont;

 return(H264SWDEC_OK);

}
