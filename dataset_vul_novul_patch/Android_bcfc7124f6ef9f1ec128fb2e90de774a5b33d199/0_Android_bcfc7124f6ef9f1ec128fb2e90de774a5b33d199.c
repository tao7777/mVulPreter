IHEVCD_ERROR_T ihevcd_cabac_init(cab_ctxt_t *ps_cabac,
 bitstrm_t *ps_bitstrm,
                                 WORD32 qp,
                                 WORD32 cabac_init_idc,
 const UWORD8 *pu1_init_ctxt)
{
 /* Sanity checks */
    ASSERT(ps_cabac != NULL);
    ASSERT(ps_bitstrm != NULL);
    ASSERT((qp >= 0) && (qp < 52));
    ASSERT((cabac_init_idc >= 0) && (cabac_init_idc < 3));
    UNUSED(qp);
    UNUSED(cabac_init_idc);
 /* CABAC engine uses 32 bit range instead of 9 bits as specified by
     * the spec. This is done to reduce number of renormalizations
     */
 /* cabac engine initialization */
#if FULLRANGE
    ps_cabac->u4_range = (UWORD32)510 << RANGE_SHIFT;
    BITS_GET(ps_cabac->u4_ofst, ps_bitstrm->pu4_buf, ps_bitstrm->u4_bit_ofst,
                    ps_bitstrm->u4_cur_word, ps_bitstrm->u4_nxt_word, (9 + RANGE_SHIFT));

#else
    ps_cabac->u4_range = (UWORD32)510;
    BITS_GET(ps_cabac->u4_ofst, ps_bitstrm->pu4_buf, ps_bitstrm->u4_bit_ofst,
                    ps_bitstrm->u4_cur_word, ps_bitstrm->u4_nxt_word, 9);

#endif

 /* cabac context initialization based on init idc and slice qp */
    memcpy(ps_cabac->au1_ctxt_models,

            pu1_init_ctxt,
            IHEVC_CAB_CTXT_END);
     DEBUG_RANGE_OFST("init", ps_cabac->u4_range, ps_cabac->u4_ofst);

    /*
     * If the offset is greater than or equal to range, return fail.
     */
    if(ps_cabac->u4_ofst >= ps_cabac->u4_range)
    {
        return ((IHEVCD_ERROR_T)IHEVCD_FAIL);
    }

     return ((IHEVCD_ERROR_T)IHEVCD_SUCCESS);
 }
