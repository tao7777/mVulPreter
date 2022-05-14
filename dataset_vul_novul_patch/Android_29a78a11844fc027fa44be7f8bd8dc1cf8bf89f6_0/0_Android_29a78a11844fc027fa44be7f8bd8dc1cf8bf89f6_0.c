IMPEG2D_ERROR_CODES_T impeg2d_init_video_state(dec_state_t *ps_dec, e_video_type_t e_video_type)
{
 /*-----------------------------------------------------------------------*/
 /* Bit Stream  that conforms to MPEG-1 <ISO/IEC 11172-2> standard        */
 /*-----------------------------------------------------------------------*/
 if(e_video_type == MPEG_1_VIDEO)
 {
        ps_dec->u2_is_mpeg2 = 0;

 /*-------------------------------------------------------------------*/
 /* force MPEG-1 parameters for proper decoder behavior               */
 /* see ISO/IEC 13818-2 section D.9.14                                */
 /*-------------------------------------------------------------------*/
        ps_dec->u2_progressive_sequence         = 1;
        ps_dec->u2_intra_dc_precision           = 0;
        ps_dec->u2_picture_structure            = FRAME_PICTURE;
        ps_dec->u2_frame_pred_frame_dct         = 1;
        ps_dec->u2_concealment_motion_vectors   = 0;
        ps_dec->u2_q_scale_type                 = 0;
        ps_dec->u2_intra_vlc_format             = 0;
        ps_dec->u2_alternate_scan               = 0;
        ps_dec->u2_repeat_first_field           = 0;

         ps_dec->u2_progressive_frame            = 1;
         ps_dec->u2_frame_rate_extension_n       = 0;
         ps_dec->u2_frame_rate_extension_d       = 0;
        ps_dec->u2_forw_f_code                  = 7;
        ps_dec->u2_back_f_code                  = 7;
 
         ps_dec->pf_vld_inv_quant                  = impeg2d_vld_inv_quant_mpeg1;
         /*-------------------------------------------------------------------*/
 /* Setting of parameters other than those mentioned in MPEG2 standard*/
 /* but used in decoding process.                                     */
 /*-------------------------------------------------------------------*/
 }
 /*-----------------------------------------------------------------------*/
 /* Bit Stream  that conforms to MPEG-2                                   */
 /*-----------------------------------------------------------------------*/
 else
 {
        ps_dec->u2_is_mpeg2                  = 1;
        ps_dec->u2_full_pel_forw_vector   = 0;
        ps_dec->u2_forw_f_code            = 7;
        ps_dec->u2_full_pel_back_vector   = 0;
        ps_dec->u2_back_f_code            = 7;
        ps_dec->pf_vld_inv_quant       = impeg2d_vld_inv_quant_mpeg2;


 }


    impeg2d_init_function_ptr(ps_dec);

 /* Set the frame Width and frame Height */
    ps_dec->u2_frame_height        = ALIGN16(ps_dec->u2_vertical_size);
    ps_dec->u2_frame_width         = ALIGN16(ps_dec->u2_horizontal_size);
    ps_dec->u2_num_horiz_mb         = (ps_dec->u2_horizontal_size + 15) >> 4;
 if (ps_dec->u2_frame_height > ps_dec->u2_create_max_height || ps_dec->u2_frame_width > ps_dec->u2_create_max_width)
 {
 return IMPEG2D_PIC_SIZE_NOT_SUPPORTED;
 }

    ps_dec->u2_num_flds_decoded = 0;

 /* Calculate the frame period */
 {
        UWORD32 numer;
        UWORD32 denom;
        numer = (UWORD32)gau2_impeg2_frm_rate_code[ps_dec->u2_frame_rate_code][1] *
 (UWORD32)(ps_dec->u2_frame_rate_extension_d + 1);

        denom = (UWORD32)gau2_impeg2_frm_rate_code[ps_dec->u2_frame_rate_code][0] *
 (UWORD32)(ps_dec->u2_frame_rate_extension_n + 1);
        ps_dec->u2_framePeriod = (numer * 1000 * 100) / denom;
 }


 if(VERTICAL_SCAN == ps_dec->u2_alternate_scan)
 {
    ps_dec->pu1_inv_scan_matrix = (UWORD8 *)gau1_impeg2_inv_scan_vertical;
 }
 else
 {
    ps_dec->pu1_inv_scan_matrix = (UWORD8 *)gau1_impeg2_inv_scan_zig_zag;
 }
 return (IMPEG2D_ERROR_CODES_T)IVD_ERROR_NONE;
}
