IHEVCD_ERROR_T ihevcd_mv_buf_mgr_add_bufs(codec_t *ps_codec)
{
    IHEVCD_ERROR_T ret = (IHEVCD_ERROR_T)IHEVCD_SUCCESS;
    WORD32 i;
    WORD32 max_dpb_size;
    WORD32 mv_bank_size_allocated;
    WORD32 pic_mv_bank_size;

 sps_t *ps_sps;
    UWORD8 *pu1_buf;
 mv_buf_t *ps_mv_buf;


 /* Initialize MV Bank buffer manager */
    ps_sps = ps_codec->s_parse.ps_sps;


 /* Compute the number of MV Bank buffers needed */
    max_dpb_size = ps_sps->ai1_sps_max_dec_pic_buffering[ps_sps->i1_sps_max_sub_layers - 1];

 /* Allocate one extra MV Bank to handle current frame
     * In case of asynchronous parsing and processing, number of buffers should increase here
     * based on when parsing and processing threads are synchronized

      */
     max_dpb_size++;
 
     pu1_buf = (UWORD8 *)ps_codec->pv_mv_bank_buf_base;
 
     ps_mv_buf = (mv_buf_t *)pu1_buf;
    pu1_buf += max_dpb_size * sizeof(mv_buf_t);
    ps_codec->ps_mv_buf = ps_mv_buf;
    mv_bank_size_allocated = ps_codec->i4_total_mv_bank_size - max_dpb_size  * sizeof(mv_buf_t);

 /* Compute MV bank size per picture */
    pic_mv_bank_size = ihevcd_get_pic_mv_bank_size(ALIGN64(ps_sps->i2_pic_width_in_luma_samples) *
                                                   ALIGN64(ps_sps->i2_pic_height_in_luma_samples));

 for(i = 0; i < max_dpb_size; i++)
 {
        WORD32 buf_ret;
        WORD32 num_pu;
        WORD32 num_ctb;
        WORD32 pic_size;
        pic_size = ALIGN64(ps_sps->i2_pic_width_in_luma_samples) *
                        ALIGN64(ps_sps->i2_pic_height_in_luma_samples);


        num_pu = pic_size / (MIN_PU_SIZE * MIN_PU_SIZE);
        num_ctb = pic_size / (MIN_CTB_SIZE * MIN_CTB_SIZE);


        mv_bank_size_allocated -= pic_mv_bank_size;

 if(mv_bank_size_allocated < 0)
 {
            ps_codec->s_parse.i4_error_code = IHEVCD_INSUFFICIENT_MEM_MVBANK;
 return IHEVCD_INSUFFICIENT_MEM_MVBANK;
 }

        ps_mv_buf->pu4_pic_pu_idx = (UWORD32 *)pu1_buf;
        pu1_buf += (num_ctb + 1) * sizeof(WORD32);

        ps_mv_buf->pu1_pic_pu_map = pu1_buf;
        pu1_buf += num_pu;

        ps_mv_buf->pu1_pic_slice_map = (UWORD16 *)pu1_buf;
        pu1_buf += ALIGN4(num_ctb * sizeof(UWORD16));

        ps_mv_buf->ps_pic_pu = (pu_t *)pu1_buf;
        pu1_buf += num_pu * sizeof(pu_t);

        buf_ret = ihevc_buf_mgr_add((buf_mgr_t *)ps_codec->pv_mv_buf_mgr, ps_mv_buf, i);

 if(0 != buf_ret)
 {
            ps_codec->s_parse.i4_error_code = IHEVCD_BUF_MGR_ERROR;
 return IHEVCD_BUF_MGR_ERROR;
 }

        ps_mv_buf++;

 }
 return ret;
}
