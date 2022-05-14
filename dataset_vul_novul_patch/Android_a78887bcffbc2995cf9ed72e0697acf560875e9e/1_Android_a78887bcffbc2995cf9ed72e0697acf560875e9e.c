WORD32 ih264d_video_decode(iv_obj_t *dec_hdl, void *pv_api_ip, void *pv_api_op)
{
 /* ! */

 dec_struct_t * ps_dec = (dec_struct_t *)(dec_hdl->pv_codec_handle);

    WORD32 i4_err_status = 0;
    UWORD8 *pu1_buf = NULL;
    WORD32 buflen;
    UWORD32 u4_max_ofst, u4_length_of_start_code = 0;

    UWORD32 bytes_consumed = 0;
    UWORD32 cur_slice_is_nonref = 0;
    UWORD32 u4_next_is_aud;
    UWORD32 u4_first_start_code_found = 0;
    WORD32 ret = 0,api_ret_value = IV_SUCCESS;
    WORD32 header_data_left = 0,frame_data_left = 0;
    UWORD8 *pu1_bitstrm_buf;
 ivd_video_decode_ip_t *ps_dec_ip;
 ivd_video_decode_op_t *ps_dec_op;

    ithread_set_name((void*)"Parse_thread");

    ps_dec_ip = (ivd_video_decode_ip_t *)pv_api_ip;
    ps_dec_op = (ivd_video_decode_op_t *)pv_api_op;

 {
        UWORD32 u4_size;
        u4_size = ps_dec_op->u4_size;
        memset(ps_dec_op, 0, sizeof(ivd_video_decode_op_t));
        ps_dec_op->u4_size = u4_size;
 }

    ps_dec->pv_dec_out = ps_dec_op;
 if(ps_dec->init_done != 1)
 {
 return IV_FAIL;
 }

 /*Data memory barries instruction,so that bitstream write by the application is complete*/
    DATA_SYNC();

 if(0 == ps_dec->u1_flushfrm)
 {
 if(ps_dec_ip->pv_stream_buffer == NULL)
 {
            ps_dec_op->u4_error_code |= 1 << IVD_UNSUPPORTEDPARAM;
            ps_dec_op->u4_error_code |= IVD_DEC_FRM_BS_BUF_NULL;
 return IV_FAIL;
 }
 if(ps_dec_ip->u4_num_Bytes <= 0)
 {
            ps_dec_op->u4_error_code |= 1 << IVD_UNSUPPORTEDPARAM;
            ps_dec_op->u4_error_code |= IVD_DEC_NUMBYTES_INV;
 return IV_FAIL;

 }
 }
    ps_dec->u1_pic_decode_done = 0;

    ps_dec_op->u4_num_bytes_consumed = 0;

    ps_dec->ps_out_buffer = NULL;

 if(ps_dec_ip->u4_size
 >= offsetof(ivd_video_decode_ip_t, s_out_buffer))
        ps_dec->ps_out_buffer = &ps_dec_ip->s_out_buffer;

    ps_dec->u4_fmt_conv_cur_row = 0;

    ps_dec->u4_output_present = 0;
    ps_dec->s_disp_op.u4_error_code = 1;
    ps_dec->u4_fmt_conv_num_rows = FMT_CONV_NUM_ROWS;
 if(0 == ps_dec->u4_share_disp_buf
 && ps_dec->i4_decode_header == 0)
 {
        UWORD32 i;
 if(ps_dec->ps_out_buffer->u4_num_bufs == 0)
 {
            ps_dec_op->u4_error_code |= 1 << IVD_UNSUPPORTEDPARAM;
            ps_dec_op->u4_error_code |= IVD_DISP_FRM_ZERO_OP_BUFS;
 return IV_FAIL;
 }

 for(i = 0; i < ps_dec->ps_out_buffer->u4_num_bufs; i++)
 {
 if(ps_dec->ps_out_buffer->pu1_bufs[i] == NULL)
 {
                ps_dec_op->u4_error_code |= 1 << IVD_UNSUPPORTEDPARAM;
                ps_dec_op->u4_error_code |= IVD_DISP_FRM_OP_BUF_NULL;
 return IV_FAIL;
 }

 if(ps_dec->ps_out_buffer->u4_min_out_buf_size[i] == 0)
 {
                ps_dec_op->u4_error_code |= 1 << IVD_UNSUPPORTEDPARAM;
                ps_dec_op->u4_error_code |=
                                IVD_DISP_FRM_ZERO_OP_BUF_SIZE;
 return IV_FAIL;
 }
 }
 }

 if(ps_dec->u4_total_frames_decoded >= NUM_FRAMES_LIMIT)
 {
        ps_dec_op->u4_error_code = ERROR_FRAME_LIMIT_OVER;
 return IV_FAIL;
 }

 /* ! */
    ps_dec->u4_ts = ps_dec_ip->u4_ts;

    ps_dec_op->u4_error_code = 0;
    ps_dec_op->e_pic_type = -1;
    ps_dec_op->u4_output_present = 0;
    ps_dec_op->u4_frame_decoded_flag = 0;

    ps_dec->i4_frametype = -1;
    ps_dec->i4_content_type = -1;
 /*
     * For field pictures, set the bottom and top picture decoded u4_flag correctly.
     */
 {
 if((TOP_FIELD_ONLY | BOT_FIELD_ONLY) == ps_dec->u1_top_bottom_decoded)
 {
            ps_dec->u1_top_bottom_decoded = 0;
 }
 }
    ps_dec->u4_slice_start_code_found = 0;

 /* In case the deocder is not in flush mode(in shared mode),
     then decoder has to pick up a buffer to write current frame.
     Check if a frame is available in such cases */

 if(ps_dec->u1_init_dec_flag == 1 && ps_dec->u4_share_disp_buf == 1
 && ps_dec->u1_flushfrm == 0)
 {
        UWORD32 i;

        WORD32 disp_avail = 0, free_id;

 /* Check if at least one buffer is available with the codec */
 /* If not then return to application with error */
 for(i = 0; i < ps_dec->u1_pic_bufs; i++)
 {
 if(0 == ps_dec->u4_disp_buf_mapping[i]
 || 1 == ps_dec->u4_disp_buf_to_be_freed[i])
 {
                disp_avail = 1;
 break;
 }

 }

 if(0 == disp_avail)
 {
 /* If something is queued for display wait for that buffer to be returned */

            ps_dec_op->u4_error_code = IVD_DEC_REF_BUF_NULL;
            ps_dec_op->u4_error_code |= (1 << IVD_UNSUPPORTEDPARAM);
 return (IV_FAIL);
 }

 while(1)
 {
 pic_buffer_t *ps_pic_buf;
            ps_pic_buf = (pic_buffer_t *)ih264_buf_mgr_get_next_free(
 (buf_mgr_t *)ps_dec->pv_pic_buf_mgr, &free_id);

 if(ps_pic_buf == NULL)
 {
                UWORD32 i, display_queued = 0;

 /* check if any buffer was given for display which is not returned yet */
 for(i = 0; i < (MAX_DISP_BUFS_NEW); i++)
 {
 if(0 != ps_dec->u4_disp_buf_mapping[i])
 {
                        display_queued = 1;
 break;
 }
 }
 /* If some buffer is queued for display, then codec has to singal an error and wait
                 for that buffer to be returned.
                 If nothing is queued for display then codec has ownership of all display buffers
                 and it can reuse any of the existing buffers and continue decoding */

 if(1 == display_queued)
 {
 /* If something is queued for display wait for that buffer to be returned */
                    ps_dec_op->u4_error_code = IVD_DEC_REF_BUF_NULL;
                    ps_dec_op->u4_error_code |= (1
 << IVD_UNSUPPORTEDPARAM);
 return (IV_FAIL);
 }
 }
 else
 {
 /* If the buffer is with display, then mark it as in use and then look for a buffer again */
 if(1 == ps_dec->u4_disp_buf_mapping[free_id])
 {
                    ih264_buf_mgr_set_status(
 (buf_mgr_t *)ps_dec->pv_pic_buf_mgr,
                                    free_id,
                                    BUF_MGR_IO);
 }
 else
 {
 /**
                     *  Found a free buffer for present call. Release it now.
                     *  Will be again obtained later.
                     */
                    ih264_buf_mgr_release((buf_mgr_t *)ps_dec->pv_pic_buf_mgr,
                                          free_id,
                                          BUF_MGR_IO);
 break;
 }
 }
 }

 }

 if(ps_dec->u1_flushfrm && ps_dec->u1_init_dec_flag)
 {

        ih264d_get_next_display_field(ps_dec, ps_dec->ps_out_buffer,
 &(ps_dec->s_disp_op));
 if(0 == ps_dec->s_disp_op.u4_error_code)
 {
            ps_dec->u4_fmt_conv_cur_row = 0;
            ps_dec->u4_fmt_conv_num_rows = ps_dec->s_disp_frame_info.u4_y_ht;
            ih264d_format_convert(ps_dec, &(ps_dec->s_disp_op),
                                  ps_dec->u4_fmt_conv_cur_row,
                                  ps_dec->u4_fmt_conv_num_rows);
            ps_dec->u4_fmt_conv_cur_row += ps_dec->u4_fmt_conv_num_rows;
            ps_dec->u4_output_present = 1;

 }
        ih264d_release_display_field(ps_dec, &(ps_dec->s_disp_op));

        ps_dec_op->u4_pic_wd = (UWORD32)ps_dec->u2_disp_width;
        ps_dec_op->u4_pic_ht = (UWORD32)ps_dec->u2_disp_height;

        ps_dec_op->u4_new_seq = 0;

        ps_dec_op->u4_output_present = ps_dec->u4_output_present;
        ps_dec_op->u4_progressive_frame_flag =
                        ps_dec->s_disp_op.u4_progressive_frame_flag;
        ps_dec_op->e_output_format =
                        ps_dec->s_disp_op.e_output_format;
        ps_dec_op->s_disp_frm_buf = ps_dec->s_disp_op.s_disp_frm_buf;
        ps_dec_op->e4_fld_type = ps_dec->s_disp_op.e4_fld_type;
        ps_dec_op->u4_ts = ps_dec->s_disp_op.u4_ts;
        ps_dec_op->u4_disp_buf_id = ps_dec->s_disp_op.u4_disp_buf_id;

 /*In the case of flush ,since no frame is decoded set pic type as invalid*/
        ps_dec_op->u4_is_ref_flag = -1;
        ps_dec_op->e_pic_type = IV_NA_FRAME;
        ps_dec_op->u4_frame_decoded_flag = 0;

 if(0 == ps_dec->s_disp_op.u4_error_code)
 {
 return (IV_SUCCESS);
 }
 else
 return (IV_FAIL);

 }
 if(ps_dec->u1_res_changed == 1)
 {
 /*if resolution has changed and all buffers have been flushed, reset decoder*/
        ih264d_init_decoder(ps_dec);
 }

    ps_dec->u4_prev_nal_skipped = 0;

    ps_dec->u2_cur_mb_addr = 0;
    ps_dec->u2_total_mbs_coded = 0;
    ps_dec->u2_cur_slice_num = 0;
    ps_dec->cur_dec_mb_num = 0;
    ps_dec->cur_recon_mb_num = 0;
    ps_dec->u4_first_slice_in_pic = 2;
    ps_dec->u1_slice_header_done = 0;
    ps_dec->u1_dangling_field = 0;


     ps_dec->u4_dec_thread_created = 0;
     ps_dec->u4_bs_deblk_thread_created = 0;
     ps_dec->u4_cur_bs_mb_num = 0;
 
     DEBUG_THREADS_PRINTF(" Starting process call\n");
 

    ps_dec->u4_pic_buf_got = 0;

 do
 {
        WORD32 buf_size;

        pu1_buf = (UWORD8*)ps_dec_ip->pv_stream_buffer
 + ps_dec_op->u4_num_bytes_consumed;

        u4_max_ofst = ps_dec_ip->u4_num_Bytes
 - ps_dec_op->u4_num_bytes_consumed;

 /* If dynamic bitstream buffer is not allocated and
         * header decode is done, then allocate dynamic bitstream buffer
         */
 if((NULL == ps_dec->pu1_bits_buf_dynamic) &&
 (ps_dec->i4_header_decoded & 1))
 {
            WORD32 size;

 void *pv_buf;
 void *pv_mem_ctxt = ps_dec->pv_mem_ctxt;
            size = MAX(256000, ps_dec->u2_pic_wd * ps_dec->u2_pic_ht * 3 / 2);
            pv_buf = ps_dec->pf_aligned_alloc(pv_mem_ctxt, 128, size);
            RETURN_IF((NULL == pv_buf), IV_FAIL);
            ps_dec->pu1_bits_buf_dynamic = pv_buf;
            ps_dec->u4_dynamic_bits_buf_size = size;
 }

 if(ps_dec->pu1_bits_buf_dynamic)
 {
            pu1_bitstrm_buf = ps_dec->pu1_bits_buf_dynamic;
            buf_size = ps_dec->u4_dynamic_bits_buf_size;
 }
 else
 {
            pu1_bitstrm_buf = ps_dec->pu1_bits_buf_static;
            buf_size = ps_dec->u4_static_bits_buf_size;
 }

        u4_next_is_aud = 0;

        buflen = ih264d_find_start_code(pu1_buf, 0, u4_max_ofst,
 &u4_length_of_start_code,
 &u4_next_is_aud);

 if(buflen == -1)
            buflen = 0;
 /* Ignore bytes beyond the allocated size of intermediate buffer */
        buflen = MIN(buflen, buf_size);

        bytes_consumed = buflen + u4_length_of_start_code;
        ps_dec_op->u4_num_bytes_consumed += bytes_consumed;

 {
            UWORD8 u1_firstbyte, u1_nal_ref_idc;

 if(ps_dec->i4_app_skip_mode == IVD_SKIP_B)
 {
                u1_firstbyte = *(pu1_buf + u4_length_of_start_code);
                u1_nal_ref_idc = (UWORD8)(NAL_REF_IDC(u1_firstbyte));
 if(u1_nal_ref_idc == 0)
 {
 /*skip non reference frames*/
                    cur_slice_is_nonref = 1;
 continue;
 }
 else
 {
 if(1 == cur_slice_is_nonref)
 {
 /*We have encountered a referenced frame,return to app*/
                        ps_dec_op->u4_num_bytes_consumed -=
                                        bytes_consumed;
                        ps_dec_op->e_pic_type = IV_B_FRAME;
                        ps_dec_op->u4_error_code =
                                        IVD_DEC_FRM_SKIPPED;
                        ps_dec_op->u4_error_code |= (1
 << IVD_UNSUPPORTEDPARAM);
                        ps_dec_op->u4_frame_decoded_flag = 0;
                        ps_dec_op->u4_size =
 sizeof(ivd_video_decode_op_t);
 /*signal the decode thread*/
                        ih264d_signal_decode_thread(ps_dec);
 /* close deblock thread if it is not closed yet*/
 if(ps_dec->u4_num_cores == 3)
 {
                            ih264d_signal_bs_deblk_thread(ps_dec);
 }

 return (IV_FAIL);
 }
 }

 }

 }


 if(buflen)
 {
            memcpy(pu1_bitstrm_buf, pu1_buf + u4_length_of_start_code,
                   buflen);
 /* Decoder may read extra 8 bytes near end of the frame */
 if((buflen + 8) < buf_size)
 {
                memset(pu1_bitstrm_buf + buflen, 0, 8);
 }
            u4_first_start_code_found = 1;

 }
 else
 {
 /*start code not found*/

 if(u4_first_start_code_found == 0)
 {
 /*no start codes found in current process call*/

                ps_dec->i4_error_code = ERROR_START_CODE_NOT_FOUND;
                ps_dec_op->u4_error_code |= 1 << IVD_INSUFFICIENTDATA;

 if(ps_dec->u4_pic_buf_got == 0)
 {

                    ih264d_fill_output_struct_from_context(ps_dec,
                                                           ps_dec_op);

                    ps_dec_op->u4_error_code = ps_dec->i4_error_code;
                    ps_dec_op->u4_frame_decoded_flag = 0;

 return (IV_FAIL);
 }
 else
 {
                    ps_dec->u1_pic_decode_done = 1;
 continue;
 }
 }
 else
 {
 /* a start code has already been found earlier in the same process call*/
                frame_data_left = 0;
 continue;
 }

 }

        ps_dec->u4_return_to_app = 0;
        ret = ih264d_parse_nal_unit(dec_hdl, ps_dec_op,
                              pu1_bitstrm_buf, buflen);
 if(ret != OK)
 {
            UWORD32 error =  ih264d_map_error(ret);
            ps_dec_op->u4_error_code = error | ret;
            api_ret_value = IV_FAIL;

 if((ret == IVD_RES_CHANGED)
 || (ret == IVD_MEM_ALLOC_FAILED)
 || (ret == ERROR_UNAVAIL_PICBUF_T)
 || (ret == ERROR_UNAVAIL_MVBUF_T))
 {
 break;
 }

 if((ret == ERROR_INCOMPLETE_FRAME) || (ret == ERROR_DANGLING_FIELD_IN_PIC))
 {
                ps_dec_op->u4_num_bytes_consumed -= bytes_consumed;
                api_ret_value = IV_FAIL;
 break;
 }

 if(ret == ERROR_IN_LAST_SLICE_OF_PIC)
 {
                api_ret_value = IV_FAIL;
 break;
 }

 }

 if(ps_dec->u4_return_to_app)
 {
 /*We have encountered a referenced frame,return to app*/
            ps_dec_op->u4_num_bytes_consumed -= bytes_consumed;
            ps_dec_op->u4_error_code = IVD_DEC_FRM_SKIPPED;
            ps_dec_op->u4_error_code |= (1 << IVD_UNSUPPORTEDPARAM);
            ps_dec_op->u4_frame_decoded_flag = 0;
            ps_dec_op->u4_size = sizeof(ivd_video_decode_op_t);
 /*signal the decode thread*/
            ih264d_signal_decode_thread(ps_dec);
 /* close deblock thread if it is not closed yet*/
 if(ps_dec->u4_num_cores == 3)
 {
                ih264d_signal_bs_deblk_thread(ps_dec);
 }
 return (IV_FAIL);

 }



        header_data_left = ((ps_dec->i4_decode_header == 1)
 && (ps_dec->i4_header_decoded != 3)
 && (ps_dec_op->u4_num_bytes_consumed
 < ps_dec_ip->u4_num_Bytes));
        frame_data_left = (((ps_dec->i4_decode_header == 0)
 && ((ps_dec->u1_pic_decode_done == 0)
 || (u4_next_is_aud == 1)))
 && (ps_dec_op->u4_num_bytes_consumed
 < ps_dec_ip->u4_num_Bytes));
 }
 while(( header_data_left == 1)||(frame_data_left == 1));

 if((ps_dec->u4_slice_start_code_found == 1)
 && (ret != IVD_MEM_ALLOC_FAILED)
 && ps_dec->u2_total_mbs_coded < ps_dec->u2_frm_ht_in_mbs * ps_dec->u2_frm_wd_in_mbs)
 {
        WORD32 num_mb_skipped;
        WORD32 prev_slice_err;
 pocstruct_t temp_poc;
        WORD32 ret1;

        num_mb_skipped = (ps_dec->u2_frm_ht_in_mbs * ps_dec->u2_frm_wd_in_mbs)
 - ps_dec->u2_total_mbs_coded;

 if(ps_dec->u4_first_slice_in_pic && (ps_dec->u4_pic_buf_got == 0))
            prev_slice_err = 1;
 else
            prev_slice_err = 2;

        ret1 = ih264d_mark_err_slice_skip(ps_dec, num_mb_skipped, ps_dec->u1_nal_unit_type == IDR_SLICE_NAL, ps_dec->ps_cur_slice->u2_frame_num,
 &temp_poc, prev_slice_err);

 if((ret1 == ERROR_UNAVAIL_PICBUF_T) || (ret1 == ERROR_UNAVAIL_MVBUF_T))
 {
 return IV_FAIL;
 }
 }

 if((ret == IVD_RES_CHANGED)
 || (ret == IVD_MEM_ALLOC_FAILED)
 || (ret == ERROR_UNAVAIL_PICBUF_T)
 || (ret == ERROR_UNAVAIL_MVBUF_T))
 {

 /* signal the decode thread */
        ih264d_signal_decode_thread(ps_dec);
 /* close deblock thread if it is not closed yet */
 if(ps_dec->u4_num_cores == 3)
 {
            ih264d_signal_bs_deblk_thread(ps_dec);
 }
 /* dont consume bitstream for change in resolution case */
 if(ret == IVD_RES_CHANGED)
 {
            ps_dec_op->u4_num_bytes_consumed -= bytes_consumed;
 }
 return IV_FAIL;
 }


 if(ps_dec->u1_separate_parse)
 {
 /* If Format conversion is not complete,
         complete it here */
 if(ps_dec->u4_num_cores == 2)
 {

 /*do deblocking of all mbs*/
 if((ps_dec->u4_nmb_deblk == 0) &&(ps_dec->u4_start_recon_deblk == 1) && (ps_dec->ps_cur_sps->u1_mb_aff_flag == 0))
 {
                UWORD32 u4_num_mbs,u4_max_addr;
 tfr_ctxt_t s_tfr_ctxt;
 tfr_ctxt_t *ps_tfr_cxt = &s_tfr_ctxt;
 pad_mgr_t *ps_pad_mgr = &ps_dec->s_pad_mgr;

 /*BS is done for all mbs while parsing*/
                u4_max_addr = (ps_dec->u2_frm_wd_in_mbs * ps_dec->u2_frm_ht_in_mbs) - 1;
                ps_dec->u4_cur_bs_mb_num = u4_max_addr + 1;


                ih264d_init_deblk_tfr_ctxt(ps_dec, ps_pad_mgr, ps_tfr_cxt,
                                           ps_dec->u2_frm_wd_in_mbs, 0);


                u4_num_mbs = u4_max_addr
 - ps_dec->u4_cur_deblk_mb_num + 1;

                DEBUG_PERF_PRINTF("mbs left for deblocking= %d \n",u4_num_mbs);

 if(u4_num_mbs != 0)
                    ih264d_check_mb_map_deblk(ps_dec, u4_num_mbs,
                                                   ps_tfr_cxt,1);

                ps_dec->u4_start_recon_deblk  = 0;

 }

 }

 /*signal the decode thread*/
        ih264d_signal_decode_thread(ps_dec);
 /* close deblock thread if it is not closed yet*/
 if(ps_dec->u4_num_cores == 3)
 {
            ih264d_signal_bs_deblk_thread(ps_dec);
 }
 }


    DATA_SYNC();


 if((ps_dec_op->u4_error_code & 0xff)
 != ERROR_DYNAMIC_RESOLUTION_NOT_SUPPORTED)
 {
        ps_dec_op->u4_pic_wd = (UWORD32)ps_dec->u2_disp_width;
        ps_dec_op->u4_pic_ht = (UWORD32)ps_dec->u2_disp_height;
 }

 if(ps_dec->i4_header_decoded != 3)
 {
        ps_dec_op->u4_error_code |= (1 << IVD_INSUFFICIENTDATA);

 }

 if(ps_dec->i4_decode_header == 1 && ps_dec->i4_header_decoded != 3)
 {
        ps_dec_op->u4_error_code |= (1 << IVD_INSUFFICIENTDATA);

 }
 if(ps_dec->u4_prev_nal_skipped)
 {
 /*We have encountered a referenced frame,return to app*/
        ps_dec_op->u4_error_code = IVD_DEC_FRM_SKIPPED;
        ps_dec_op->u4_error_code |= (1 << IVD_UNSUPPORTEDPARAM);
        ps_dec_op->u4_frame_decoded_flag = 0;
        ps_dec_op->u4_size = sizeof(ivd_video_decode_op_t);
 /* close deblock thread if it is not closed yet*/
 if(ps_dec->u4_num_cores == 3)
 {
            ih264d_signal_bs_deblk_thread(ps_dec);
 }
 return (IV_FAIL);

 }

 if((ps_dec->u4_slice_start_code_found == 1)
 && (ERROR_DANGLING_FIELD_IN_PIC != i4_err_status))
 {
 /*
         * For field pictures, set the bottom and top picture decoded u4_flag correctly.
         */

 if(ps_dec->ps_cur_slice->u1_field_pic_flag)
 {
 if(1 == ps_dec->ps_cur_slice->u1_bottom_field_flag)
 {
                ps_dec->u1_top_bottom_decoded |= BOT_FIELD_ONLY;
 }
 else
 {
                ps_dec->u1_top_bottom_decoded |= TOP_FIELD_ONLY;
 }
 }

 /* if new frame in not found (if we are still getting slices from previous frame)
         * ih264d_deblock_display is not called. Such frames will not be added to reference /display
         */
 if((ps_dec->ps_dec_err_status->u1_err_flag & REJECT_CUR_PIC) == 0)
 {
 /* Calling Function to deblock Picture and Display */
            ret = ih264d_deblock_display(ps_dec);
 if(ret != 0)
 {
 return IV_FAIL;
 }
 }


 /*set to complete ,as we dont support partial frame decode*/
 if(ps_dec->i4_header_decoded == 3)
 {
            ps_dec->u2_total_mbs_coded = ps_dec->ps_cur_sps->u2_max_mb_addr + 1;
 }

 /*Update the i4_frametype at the end of picture*/
 if(ps_dec->ps_cur_slice->u1_nal_unit_type == IDR_SLICE_NAL)
 {
            ps_dec->i4_frametype = IV_IDR_FRAME;
 }
 else if(ps_dec->i4_pic_type == B_SLICE)
 {
            ps_dec->i4_frametype = IV_B_FRAME;
 }
 else if(ps_dec->i4_pic_type == P_SLICE)
 {
            ps_dec->i4_frametype = IV_P_FRAME;
 }
 else if(ps_dec->i4_pic_type == I_SLICE)
 {
            ps_dec->i4_frametype = IV_I_FRAME;
 }
 else
 {
            H264_DEC_DEBUG_PRINT("Shouldn't come here\n");
 }

        ps_dec->i4_content_type = ps_dec->ps_cur_slice->u1_field_pic_flag;

        ps_dec->u4_total_frames_decoded = ps_dec->u4_total_frames_decoded + 2;
        ps_dec->u4_total_frames_decoded = ps_dec->u4_total_frames_decoded
 - ps_dec->ps_cur_slice->u1_field_pic_flag;

 }

 /* close deblock thread if it is not closed yet*/
 if(ps_dec->u4_num_cores == 3)
 {
        ih264d_signal_bs_deblk_thread(ps_dec);
 }


 {
 /* In case the decoder is configured to run in low delay mode,
         * then get display buffer and then format convert.
         * Note in this mode, format conversion does not run paralelly in a thread and adds to the codec cycles
         */

 if((IVD_DECODE_FRAME_OUT == ps_dec->e_frm_out_mode)
 && ps_dec->u1_init_dec_flag)
 {

            ih264d_get_next_display_field(ps_dec, ps_dec->ps_out_buffer,
 &(ps_dec->s_disp_op));
 if(0 == ps_dec->s_disp_op.u4_error_code)
 {
                ps_dec->u4_fmt_conv_cur_row = 0;
                ps_dec->u4_output_present = 1;
 }
 }

        ih264d_fill_output_struct_from_context(ps_dec, ps_dec_op);

 /* If Format conversion is not complete,
         complete it here */
 if(ps_dec->u4_output_present &&
 (ps_dec->u4_fmt_conv_cur_row < ps_dec->s_disp_frame_info.u4_y_ht))
 {
            ps_dec->u4_fmt_conv_num_rows = ps_dec->s_disp_frame_info.u4_y_ht
 - ps_dec->u4_fmt_conv_cur_row;
            ih264d_format_convert(ps_dec, &(ps_dec->s_disp_op),
                                  ps_dec->u4_fmt_conv_cur_row,
                                  ps_dec->u4_fmt_conv_num_rows);
            ps_dec->u4_fmt_conv_cur_row += ps_dec->u4_fmt_conv_num_rows;
 }

        ih264d_release_display_field(ps_dec, &(ps_dec->s_disp_op));
 }

 if(ps_dec->i4_decode_header == 1 && (ps_dec->i4_header_decoded & 1) == 1)
 {
        ps_dec_op->u4_progressive_frame_flag = 1;
 if((NULL != ps_dec->ps_cur_sps) && (1 == (ps_dec->ps_cur_sps->u1_is_valid)))
 {
 if((0 == ps_dec->ps_sps->u1_frame_mbs_only_flag)
 && (0 == ps_dec->ps_sps->u1_mb_aff_flag))
                ps_dec_op->u4_progressive_frame_flag = 0;

 }
 }

 /*Data memory barrier instruction,so that yuv write by the library is complete*/
    DATA_SYNC();

    H264_DEC_DEBUG_PRINT("The num bytes consumed: %d\n",
                         ps_dec_op->u4_num_bytes_consumed);
 return api_ret_value;
}
