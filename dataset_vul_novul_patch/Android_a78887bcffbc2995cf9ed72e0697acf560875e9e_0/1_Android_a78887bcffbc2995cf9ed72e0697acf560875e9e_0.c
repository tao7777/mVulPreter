WORD32 ih264d_parse_nal_unit(iv_obj_t *dec_hdl,
 ivd_video_decode_op_t *ps_dec_op,
                          UWORD8 *pu1_buf,
                          UWORD32 u4_length)
{

 dec_bit_stream_t *ps_bitstrm;


 dec_struct_t *ps_dec = (dec_struct_t *)dec_hdl->pv_codec_handle;
 ivd_video_decode_ip_t *ps_dec_in =
 (ivd_video_decode_ip_t *)ps_dec->pv_dec_in;
 dec_slice_params_t * ps_cur_slice = ps_dec->ps_cur_slice;
    UWORD8 u1_first_byte, u1_nal_ref_idc;
    UWORD8 u1_nal_unit_type;
    WORD32 i_status = OK;
    ps_bitstrm = ps_dec->ps_bitstrm;

 if(pu1_buf)
 {
 if(u4_length)
 {
            ps_dec_op->u4_frame_decoded_flag = 0;
            ih264d_process_nal_unit(ps_dec->ps_bitstrm, pu1_buf,
                                    u4_length);

            SWITCHOFFTRACE;
            u1_first_byte = ih264d_get_bits_h264(ps_bitstrm, 8);

 if(NAL_FORBIDDEN_BIT(u1_first_byte))
 {

                 H264_DEC_DEBUG_PRINT("\nForbidden bit set in Nal Unit, Let's try\n");
             }
             u1_nal_unit_type = NAL_UNIT_TYPE(u1_first_byte);
             ps_dec->u1_nal_unit_type = u1_nal_unit_type;
             u1_nal_ref_idc = (UWORD8)(NAL_REF_IDC(u1_first_byte));
 switch(u1_nal_unit_type)
 {
 case SLICE_DATA_PARTITION_A_NAL:
 case SLICE_DATA_PARTITION_B_NAL:
 case SLICE_DATA_PARTITION_C_NAL:
 if(!ps_dec->i4_decode_header)
                        ih264d_parse_slice_partition(ps_dec, ps_bitstrm);

 break;

 case IDR_SLICE_NAL:
 case SLICE_NAL:

 /* ! */
                    DEBUG_THREADS_PRINTF("Decoding  a slice NAL\n");
 if(!ps_dec->i4_decode_header)
 {
 if(ps_dec->i4_header_decoded == 3)
 {
 /* ! */
                            ps_dec->u4_slice_start_code_found = 1;

                            ih264d_rbsp_to_sodb(ps_dec->ps_bitstrm);

                            i_status = ih264d_parse_decode_slice(
 (UWORD8)(u1_nal_unit_type
 == IDR_SLICE_NAL),
                                            u1_nal_ref_idc, ps_dec);

 if((ps_dec->u4_first_slice_in_pic != 0)&&
 ((ps_dec->ps_dec_err_status->u1_err_flag & REJECT_CUR_PIC) == 0))
 {
 /*  if the first slice header was not valid set to 1 */
                                ps_dec->u4_first_slice_in_pic = 1;
 }

 if(i_status != OK)
 {
 return i_status;
 }
 }
 else
 {
                            H264_DEC_DEBUG_PRINT(
 "\nSlice NAL Supplied but no header has been supplied\n");
 }
 }
 break;

 case SEI_NAL:
 if(!ps_dec->i4_decode_header)
 {
                        ih264d_rbsp_to_sodb(ps_dec->ps_bitstrm);
                        i_status = ih264d_parse_sei_message(ps_dec, ps_bitstrm);
 if(i_status != OK)
 return i_status;
                        ih264d_parse_sei(ps_dec, ps_bitstrm);
 }
 break;
 case SEQ_PARAM_NAL:
 /* ! */
                    ih264d_rbsp_to_sodb(ps_dec->ps_bitstrm);
                    i_status = ih264d_parse_sps(ps_dec, ps_bitstrm);
 if(i_status == ERROR_INV_SPS_PPS_T)
 return i_status;
 if(!i_status)
                        ps_dec->i4_header_decoded |= 0x1;
 break;

 case PIC_PARAM_NAL:
 /* ! */
                    ih264d_rbsp_to_sodb(ps_dec->ps_bitstrm);
                    i_status = ih264d_parse_pps(ps_dec, ps_bitstrm);
 if(i_status == ERROR_INV_SPS_PPS_T)
 return i_status;
 if(!i_status)
                        ps_dec->i4_header_decoded |= 0x2;
 break;
 case ACCESS_UNIT_DELIMITER_RBSP:
 if(!ps_dec->i4_decode_header)
 {
                        ih264d_access_unit_delimiter_rbsp(ps_dec);
 }
 break;
 case END_OF_STREAM_RBSP:
 if(!ps_dec->i4_decode_header)
 {
                        ih264d_parse_end_of_stream(ps_dec);
 }
 break;
 case FILLER_DATA_NAL:
 if(!ps_dec->i4_decode_header)
 {
                        ih264d_parse_filler_data(ps_dec, ps_bitstrm);
 }
 break;
 default:
                    H264_DEC_DEBUG_PRINT("\nUnknown NAL type %d\n", u1_nal_unit_type);
 break;
 }

 }

 }

 return i_status;

}
