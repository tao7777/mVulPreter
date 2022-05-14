IV_API_CALL_STATUS_T impeg2d_api_entity(iv_obj_t *ps_dechdl,
 void *pv_api_ip,
 void *pv_api_op)
{
 iv_obj_t *ps_dec_handle;
 dec_state_t *ps_dec_state;
 dec_state_multi_core_t *ps_dec_state_multi_core;

 impeg2d_video_decode_ip_t *ps_dec_ip;

 impeg2d_video_decode_op_t *ps_dec_op;
    WORD32 bytes_remaining;
 pic_buf_t *ps_disp_pic;



    ps_dec_ip = (impeg2d_video_decode_ip_t *)pv_api_ip;
    ps_dec_op = (impeg2d_video_decode_op_t *)pv_api_op;

    memset(ps_dec_op,0,sizeof(impeg2d_video_decode_op_t));

    ps_dec_op->s_ivd_video_decode_op_t.u4_size = sizeof(impeg2d_video_decode_op_t);
    ps_dec_op->s_ivd_video_decode_op_t.u4_output_present = 0;
    bytes_remaining = ps_dec_ip->s_ivd_video_decode_ip_t.u4_num_Bytes;

    ps_dec_handle = (iv_obj_t *)ps_dechdl;

 if(ps_dechdl == NULL)
 {
 return(IV_FAIL);
 }



    ps_dec_state_multi_core  = ps_dec_handle->pv_codec_handle;
    ps_dec_state = ps_dec_state_multi_core->ps_dec_state[0];

    ps_dec_state->ps_disp_frm_buf = &(ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf);
 if(0 == ps_dec_state->u4_share_disp_buf)
 {
        ps_dec_state->ps_disp_frm_buf->pv_y_buf  = ps_dec_ip->s_ivd_video_decode_ip_t.s_out_buffer.pu1_bufs[0];
        ps_dec_state->ps_disp_frm_buf->pv_u_buf  = ps_dec_ip->s_ivd_video_decode_ip_t.s_out_buffer.pu1_bufs[1];
        ps_dec_state->ps_disp_frm_buf->pv_v_buf  = ps_dec_ip->s_ivd_video_decode_ip_t.s_out_buffer.pu1_bufs[2];
 }

    ps_dec_state->ps_disp_pic = NULL;
    ps_dec_state->i4_frame_decoded = 0;
 /*rest bytes consumed */
    ps_dec_op->s_ivd_video_decode_op_t.u4_num_bytes_consumed = 0;

    ps_dec_op->s_ivd_video_decode_op_t.u4_error_code           = IV_SUCCESS;

 if((ps_dec_ip->s_ivd_video_decode_ip_t.pv_stream_buffer == NULL)&&(ps_dec_state->u1_flushfrm==0))
 {
        ps_dec_op->s_ivd_video_decode_op_t.u4_error_code |= 1 << IVD_UNSUPPORTEDPARAM;
        ps_dec_op->s_ivd_video_decode_op_t.u4_error_code |= IVD_DEC_FRM_BS_BUF_NULL;
 return IV_FAIL;
 }


 if (ps_dec_state->u4_num_frames_decoded > NUM_FRAMES_LIMIT)
 {
        ps_dec_op->s_ivd_video_decode_op_t.u4_error_code       = IMPEG2D_SAMPLE_VERSION_LIMIT_ERR;
 return(IV_FAIL);
 }

 if(((0 == ps_dec_state->u2_header_done) || (ps_dec_state->u2_decode_header == 1)) && (ps_dec_state->u1_flushfrm == 0))
 {
        impeg2d_dec_hdr(ps_dec_state,ps_dec_ip ,ps_dec_op);

         bytes_remaining -= ps_dec_op->s_ivd_video_decode_op_t.u4_num_bytes_consumed;
     }
 
    if((1 != ps_dec_state->u2_decode_header) &&
        (((bytes_remaining > 0) && (1 == ps_dec_state->u2_header_done)) || ps_dec_state->u1_flushfrm))
     {
         if(ps_dec_state->u1_flushfrm)
         {
 if(ps_dec_state->aps_ref_pics[1] != NULL)
 {
                impeg2_disp_mgr_add(&ps_dec_state->s_disp_mgr, ps_dec_state->aps_ref_pics[1], ps_dec_state->aps_ref_pics[1]->i4_buf_id);
                impeg2_buf_mgr_release(ps_dec_state->pv_pic_buf_mg, ps_dec_state->aps_ref_pics[1]->i4_buf_id, BUF_MGR_REF);
                impeg2_buf_mgr_release(ps_dec_state->pv_pic_buf_mg, ps_dec_state->aps_ref_pics[0]->i4_buf_id, BUF_MGR_REF);

                ps_dec_state->aps_ref_pics[1] = NULL;
                ps_dec_state->aps_ref_pics[0] = NULL;

 }
 else if(ps_dec_state->aps_ref_pics[0] != NULL)
 {
                impeg2_disp_mgr_add(&ps_dec_state->s_disp_mgr, ps_dec_state->aps_ref_pics[0], ps_dec_state->aps_ref_pics[0]->i4_buf_id);
                impeg2_buf_mgr_release(ps_dec_state->pv_pic_buf_mg, ps_dec_state->aps_ref_pics[0]->i4_buf_id, BUF_MGR_REF);

                ps_dec_state->aps_ref_pics[0] = NULL;
 }
            ps_dec_ip->s_ivd_video_decode_ip_t.u4_size                 = sizeof(impeg2d_video_decode_ip_t);
            ps_dec_op->s_ivd_video_decode_op_t.u4_size                 = sizeof(impeg2d_video_decode_op_t);

            ps_disp_pic = impeg2_disp_mgr_get(&ps_dec_state->s_disp_mgr, &ps_dec_state->i4_disp_buf_id);

            ps_dec_state->ps_disp_pic = ps_disp_pic;
 if(ps_disp_pic == NULL)
 {
                ps_dec_op->s_ivd_video_decode_op_t.u4_output_present = 0;
 }
 else
 {
                WORD32 fmt_conv;
 if(0 == ps_dec_state->u4_share_disp_buf)
 {
                    ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.pv_y_buf  = ps_dec_ip->s_ivd_video_decode_ip_t.s_out_buffer.pu1_bufs[0];
                    ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.pv_u_buf  = ps_dec_ip->s_ivd_video_decode_ip_t.s_out_buffer.pu1_bufs[1];
                    ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.pv_v_buf  = ps_dec_ip->s_ivd_video_decode_ip_t.s_out_buffer.pu1_bufs[2];
                    fmt_conv = 1;
 }
 else
 {
                    ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.pv_y_buf  = ps_disp_pic->pu1_y;
 if(IV_YUV_420P == ps_dec_state->i4_chromaFormat)
 {
                        ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.pv_u_buf  = ps_disp_pic->pu1_u;
                        ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.pv_v_buf  = ps_disp_pic->pu1_v;
                        fmt_conv = 0;
 }
 else
 {
                        UWORD8 *pu1_buf;

                        pu1_buf = ps_dec_state->as_disp_buffers[ps_disp_pic->i4_buf_id].pu1_bufs[1];
                        ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.pv_u_buf  = pu1_buf;

                        pu1_buf = ps_dec_state->as_disp_buffers[ps_disp_pic->i4_buf_id].pu1_bufs[2];
                        ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.pv_v_buf  = pu1_buf;
                        fmt_conv = 1;
 }
 }

 if(fmt_conv == 1)
 {
 iv_yuv_buf_t *ps_dst;


                    ps_dst = &(ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf);
 if(ps_dec_state->u4_deinterlace && (0 == ps_dec_state->u2_progressive_frame))
 {
                        impeg2d_deinterlace(ps_dec_state,
                                            ps_disp_pic,
                                            ps_dst,
 0,
                                            ps_dec_state->u2_vertical_size);

 }
 else
 {
                        impeg2d_format_convert(ps_dec_state,
                                               ps_disp_pic,
                                               ps_dst,
 0,
                                               ps_dec_state->u2_vertical_size);
 }
 }

 if(ps_dec_state->u4_deinterlace)
 {
 if(ps_dec_state->ps_deint_pic)
 {
                        impeg2_buf_mgr_release(ps_dec_state->pv_pic_buf_mg,
                                               ps_dec_state->ps_deint_pic->i4_buf_id,
                                               MPEG2_BUF_MGR_DEINT);
 }
                    ps_dec_state->ps_deint_pic = ps_disp_pic;
 }
 if(0 == ps_dec_state->u4_share_disp_buf)
                    impeg2_buf_mgr_release(ps_dec_state->pv_pic_buf_mg, ps_disp_pic->i4_buf_id, BUF_MGR_DISP);

                ps_dec_op->s_ivd_video_decode_op_t.u4_pic_ht = ps_dec_state->u2_vertical_size;
                ps_dec_op->s_ivd_video_decode_op_t.u4_pic_wd = ps_dec_state->u2_horizontal_size;
                ps_dec_op->s_ivd_video_decode_op_t.u4_output_present = 1;

                ps_dec_op->s_ivd_video_decode_op_t.u4_disp_buf_id = ps_disp_pic->i4_buf_id;
                ps_dec_op->s_ivd_video_decode_op_t.u4_ts = ps_disp_pic->u4_ts;

                ps_dec_op->s_ivd_video_decode_op_t.e_output_format = (IV_COLOR_FORMAT_T)ps_dec_state->i4_chromaFormat;

                ps_dec_op->s_ivd_video_decode_op_t.u4_is_ref_flag = (B_PIC != ps_dec_state->e_pic_type);

                ps_dec_op->s_ivd_video_decode_op_t.u4_progressive_frame_flag           = IV_PROGRESSIVE;

                ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.u4_y_wd = ps_dec_state->u2_horizontal_size;
                ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.u4_y_strd = ps_dec_state->u4_frm_buf_stride;
                ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.u4_y_ht = ps_dec_state->u2_vertical_size;

                ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.u4_u_wd = ps_dec_state->u2_horizontal_size >> 1;
                ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.u4_u_strd = ps_dec_state->u4_frm_buf_stride >> 1;
                ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.u4_u_ht = ps_dec_state->u2_vertical_size >> 1;

                ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.u4_v_wd = ps_dec_state->u2_horizontal_size >> 1;
                ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.u4_v_strd = ps_dec_state->u4_frm_buf_stride >> 1;
                ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.u4_v_ht = ps_dec_state->u2_vertical_size >> 1;
                ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.u4_size = sizeof(ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf);

 switch(ps_dec_state->i4_chromaFormat)
 {
 case IV_YUV_420SP_UV:
 case IV_YUV_420SP_VU:
                        ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.u4_u_wd = ps_dec_state->u2_horizontal_size;
                        ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.u4_u_strd = ps_dec_state->u4_frm_buf_stride;
 break;
 case IV_YUV_422ILE:
                        ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.u4_u_wd = 0;
                        ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.u4_u_ht = 0;
                        ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.u4_v_wd = 0;
                        ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.u4_v_ht = 0;
 break;
 default:
 break;
 }


 }
 if(ps_dec_op->s_ivd_video_decode_op_t.u4_output_present)
 {
 if(1 == ps_dec_op->s_ivd_video_decode_op_t.u4_output_present)
 {
                    INSERT_LOGO(ps_dec_ip->s_ivd_video_decode_ip_t.s_out_buffer.pu1_bufs[0],
                                ps_dec_ip->s_ivd_video_decode_ip_t.s_out_buffer.pu1_bufs[1],
                                ps_dec_ip->s_ivd_video_decode_ip_t.s_out_buffer.pu1_bufs[2],
                                ps_dec_state->u4_frm_buf_stride,
                                ps_dec_state->u2_horizontal_size,
                                ps_dec_state->u2_vertical_size,
                                ps_dec_state->i4_chromaFormat,
                                ps_dec_state->u2_horizontal_size,
                                ps_dec_state->u2_vertical_size);
 }
 return(IV_SUCCESS);
 }
 else
 {
                ps_dec_state->u1_flushfrm = 0;

 return(IV_FAIL);
 }

 }
 else if(ps_dec_state->u1_flushfrm==0)
 {
            ps_dec_ip->s_ivd_video_decode_ip_t.u4_size                 = sizeof(impeg2d_video_decode_ip_t);
            ps_dec_op->s_ivd_video_decode_op_t.u4_size                 = sizeof(impeg2d_video_decode_op_t);
 if(ps_dec_ip->s_ivd_video_decode_ip_t.u4_num_Bytes < 4)
 {
                ps_dec_op->s_ivd_video_decode_op_t.u4_num_bytes_consumed = ps_dec_ip->s_ivd_video_decode_ip_t.u4_num_Bytes;
 return(IV_FAIL);
 }

 if(1 == ps_dec_state->u4_share_disp_buf)
 {
 if(0 == impeg2_buf_mgr_check_free(ps_dec_state->pv_pic_buf_mg))
 {
                    ps_dec_op->s_ivd_video_decode_op_t.u4_error_code =
 (IMPEG2D_ERROR_CODES_T)IVD_DEC_REF_BUF_NULL;
 return IV_FAIL;
 }
 }


            ps_dec_op->s_ivd_video_decode_op_t.e_output_format = (IV_COLOR_FORMAT_T)ps_dec_state->i4_chromaFormat;

            ps_dec_op->s_ivd_video_decode_op_t.u4_is_ref_flag = (B_PIC != ps_dec_state->e_pic_type);

            ps_dec_op->s_ivd_video_decode_op_t.u4_progressive_frame_flag           = IV_PROGRESSIVE;

 if (0 == ps_dec_state->u4_frm_buf_stride)
 {
                ps_dec_state->u4_frm_buf_stride = (ps_dec_state->u2_horizontal_size);
 }

            ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.u4_y_wd = ps_dec_state->u2_horizontal_size;
            ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.u4_y_strd = ps_dec_state->u4_frm_buf_stride;
            ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.u4_y_ht = ps_dec_state->u2_vertical_size;

            ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.u4_u_wd = ps_dec_state->u2_horizontal_size >> 1;
            ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.u4_u_strd = ps_dec_state->u4_frm_buf_stride >> 1;
            ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.u4_u_ht = ps_dec_state->u2_vertical_size >> 1;

            ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.u4_v_wd = ps_dec_state->u2_horizontal_size >> 1;
            ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.u4_v_strd = ps_dec_state->u4_frm_buf_stride >> 1;
            ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.u4_v_ht = ps_dec_state->u2_vertical_size >> 1;
            ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.u4_size = sizeof(ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf);

 switch(ps_dec_state->i4_chromaFormat)
 {
 case IV_YUV_420SP_UV:
 case IV_YUV_420SP_VU:
                    ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.u4_u_wd = ps_dec_state->u2_horizontal_size;
                    ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.u4_u_strd = ps_dec_state->u4_frm_buf_stride;
 break;
 case IV_YUV_422ILE:
                    ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.u4_u_wd = 0;
                    ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.u4_u_ht = 0;
                    ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.u4_v_wd = 0;
                    ps_dec_op->s_ivd_video_decode_op_t.s_disp_frm_buf.u4_v_ht = 0;
 break;
 default:
 break;
 }

 if( ps_dec_state->u1_flushfrm == 0)
 {
                ps_dec_state->u1_flushcnt    = 0;

 /*************************************************************************/
 /*                              Frame Decode                             */
 /*************************************************************************/

                impeg2d_dec_frm(ps_dec_state,ps_dec_ip,ps_dec_op);

 if (IVD_ERROR_NONE ==
                        ps_dec_op->s_ivd_video_decode_op_t.u4_error_code)
 {
 if(ps_dec_state->u1_first_frame_done == 0)
 {
                        ps_dec_state->u1_first_frame_done = 1;
 }

 if(ps_dec_state->ps_disp_pic)
 {
                        ps_dec_op->s_ivd_video_decode_op_t.u4_output_present = 1;
 switch(ps_dec_state->ps_disp_pic->e_pic_type)
 {
 case I_PIC :
                            ps_dec_op->s_ivd_video_decode_op_t.e_pic_type = IV_I_FRAME;
 break;

 case P_PIC:
                            ps_dec_op->s_ivd_video_decode_op_t.e_pic_type = IV_P_FRAME;
 break;

 case B_PIC:
                            ps_dec_op->s_ivd_video_decode_op_t.e_pic_type = IV_B_FRAME;
 break;

 case D_PIC:
                            ps_dec_op->s_ivd_video_decode_op_t.e_pic_type = IV_I_FRAME;
 break;

 default :
                            ps_dec_op->s_ivd_video_decode_op_t.e_pic_type = IV_FRAMETYPE_DEFAULT;
 break;
 }
 }
 else
 {
                        ps_dec_op->s_ivd_video_decode_op_t.u4_output_present = 0;
                        ps_dec_op->s_ivd_video_decode_op_t.e_pic_type = IV_NA_FRAME;
 }

                    ps_dec_state->u4_num_frames_decoded++;
 }
 }
 else
 {
                ps_dec_state->u1_flushcnt++;
 }
 }
 if(ps_dec_state->ps_disp_pic)
 {
            ps_dec_op->s_ivd_video_decode_op_t.u4_disp_buf_id = ps_dec_state->ps_disp_pic->i4_buf_id;
            ps_dec_op->s_ivd_video_decode_op_t.u4_ts = ps_dec_state->ps_disp_pic->u4_ts;

 if(0 == ps_dec_state->u4_share_disp_buf)
 {
                impeg2_buf_mgr_release(ps_dec_state->pv_pic_buf_mg, ps_dec_state->ps_disp_pic->i4_buf_id, BUF_MGR_DISP);
 }
 }

 if(ps_dec_state->u4_deinterlace)
 {
 if(ps_dec_state->ps_deint_pic)
 {
                impeg2_buf_mgr_release(ps_dec_state->pv_pic_buf_mg,
                                       ps_dec_state->ps_deint_pic->i4_buf_id,
                                       MPEG2_BUF_MGR_DEINT);
 }
            ps_dec_state->ps_deint_pic = ps_dec_state->ps_disp_pic;
 }

 if(1 == ps_dec_op->s_ivd_video_decode_op_t.u4_output_present)
 {
            INSERT_LOGO(ps_dec_ip->s_ivd_video_decode_ip_t.s_out_buffer.pu1_bufs[0],
                        ps_dec_ip->s_ivd_video_decode_ip_t.s_out_buffer.pu1_bufs[1],
                        ps_dec_ip->s_ivd_video_decode_ip_t.s_out_buffer.pu1_bufs[2],
                        ps_dec_state->u4_frm_buf_stride,
                        ps_dec_state->u2_horizontal_size,
                        ps_dec_state->u2_vertical_size,
                        ps_dec_state->i4_chromaFormat,
                        ps_dec_state->u2_horizontal_size,
                        ps_dec_state->u2_vertical_size);
 }

 }

    ps_dec_op->s_ivd_video_decode_op_t.u4_progressive_frame_flag = 1;
    ps_dec_op->s_ivd_video_decode_op_t.e4_fld_type     = ps_dec_state->s_disp_op.e4_fld_type;


 if(ps_dec_op->s_ivd_video_decode_op_t.u4_error_code)
 return IV_FAIL;
 else
 return IV_SUCCESS;
}
