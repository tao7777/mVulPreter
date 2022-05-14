IMPEG2D_ERROR_CODES_T impeg2d_dec_pic_hdr(dec_state_t *ps_dec)
{
 stream_t *ps_stream;
    ps_stream = &ps_dec->s_bit_stream;

    impeg2d_bit_stream_flush(ps_stream,START_CODE_LEN);
 /* Flush temporal reference */
    impeg2d_bit_stream_get(ps_stream,10);

 /* Picture type */
    ps_dec->e_pic_type = (e_pic_type_t)impeg2d_bit_stream_get(ps_stream,3);
 if((ps_dec->e_pic_type < I_PIC) || (ps_dec->e_pic_type > D_PIC))
 {
        impeg2d_next_code(ps_dec, PICTURE_START_CODE);
 return IMPEG2D_INVALID_PIC_TYPE;
 }

 /* Flush vbv_delay */
    impeg2d_bit_stream_get(ps_stream,16);

 if(ps_dec->e_pic_type == P_PIC || ps_dec->e_pic_type == B_PIC)
 {
        ps_dec->u2_full_pel_forw_vector = impeg2d_bit_stream_get_bit(ps_stream);
        ps_dec->u2_forw_f_code          = impeg2d_bit_stream_get(ps_stream,3);
 }
 if(ps_dec->e_pic_type == B_PIC)
 {
        ps_dec->u2_full_pel_back_vector = impeg2d_bit_stream_get_bit(ps_stream);
        ps_dec->u2_back_f_code          = impeg2d_bit_stream_get(ps_stream,3);
 }

 
     if(ps_dec->u2_is_mpeg2 == 0)
     {
         ps_dec->au2_f_code[0][0] = ps_dec->au2_f_code[0][1] = ps_dec->u2_forw_f_code;
         ps_dec->au2_f_code[1][0] = ps_dec->au2_f_code[1][1] = ps_dec->u2_back_f_code;
     }

 /*-----------------------------------------------------------------------*/
 /*  Flush the extra bit value                                            */
 /*                                                                       */
 /*  while(impeg2d_bit_stream_nxt() == '1')                                  */
 /*  {                                                                    */
 /*      extra_bit_picture         1                                      */
 /*      extra_information_picture 8                                      */
 /*  }                                                                    */
 /*  extra_bit_picture             1                                      */
 /*-----------------------------------------------------------------------*/
 while (impeg2d_bit_stream_nxt(ps_stream,1) == 1 &&
           ps_stream->u4_offset < ps_stream->u4_max_offset)
 {
        impeg2d_bit_stream_get(ps_stream,9);
 }
    impeg2d_bit_stream_get_bit(ps_stream);
    impeg2d_next_start_code(ps_dec);

 return (IMPEG2D_ERROR_CODES_T)IVD_ERROR_NONE;
}
