void ihevcd_parse_sei_payload(codec_t *ps_codec,
                              UWORD32 u4_payload_type,
                              UWORD32 u4_payload_size,
                              WORD8 i1_nal_type)
{
 parse_ctxt_t *ps_parse = &ps_codec->s_parse;
 bitstrm_t *ps_bitstrm = &ps_parse->s_bitstrm;
    WORD32 payload_bits_remaining = 0;
 sps_t *ps_sps;

    UWORD32 i;

 for(i = 0; i < MAX_SPS_CNT; i++)
 {
        ps_sps = ps_codec->ps_sps_base + i;
 if(ps_sps->i1_sps_valid)
 {
 break;
 }
 }
 if(NULL == ps_sps)
 {
 return;
 }

 if(NAL_PREFIX_SEI == i1_nal_type)
 {
 switch(u4_payload_type)
 {
 case SEI_BUFFERING_PERIOD:
                ps_parse->s_sei_params.i1_sei_parameters_present_flag = 1;
                ihevcd_parse_buffering_period_sei(ps_codec, ps_sps);
 break;

 case SEI_PICTURE_TIMING:
                ps_parse->s_sei_params.i1_sei_parameters_present_flag = 1;
                ihevcd_parse_pic_timing_sei(ps_codec, ps_sps);
 break;

 case SEI_TIME_CODE:
                ps_parse->s_sei_params.i1_sei_parameters_present_flag = 1;
                ihevcd_parse_time_code_sei(ps_codec);
 break;

 case SEI_MASTERING_DISPLAY_COLOUR_VOLUME:
                ps_parse->s_sei_params.i4_sei_mastering_disp_colour_vol_params_present_flags = 1;
                ihevcd_parse_mastering_disp_params_sei(ps_codec);
 break;

 
             case SEI_USER_DATA_REGISTERED_ITU_T_T35:
                 ps_parse->s_sei_params.i1_sei_parameters_present_flag = 1;
                if(ps_parse->s_sei_params.i4_sei_user_data_cnt >= USER_DATA_MAX)
                {
                    for(i = 0; i < u4_payload_size / 4; i++)
                    {
                        ihevcd_bits_flush(ps_bitstrm, 4 * 8);
                    }

                    ihevcd_bits_flush(ps_bitstrm, (u4_payload_size - i * 4) * 8);
                }
                else
                {
                    ihevcd_parse_user_data_registered_itu_t_t35(ps_codec,
                                                                u4_payload_size);
                }
                 break;
 
             default:
 for(i = 0; i < u4_payload_size; i++)
 {
                    ihevcd_bits_flush(ps_bitstrm, 8);
 }
 break;
 }
 }
 else /* NAL_SUFFIX_SEI */
 {
 switch(u4_payload_type)

         {
             case SEI_USER_DATA_REGISTERED_ITU_T_T35:
                 ps_parse->s_sei_params.i1_sei_parameters_present_flag = 1;
                if(ps_parse->s_sei_params.i4_sei_user_data_cnt >= USER_DATA_MAX)
                {
                    for(i = 0; i < u4_payload_size / 4; i++)
                    {
                        ihevcd_bits_flush(ps_bitstrm, 4 * 8);
                    }

                    ihevcd_bits_flush(ps_bitstrm, (u4_payload_size - i * 4) * 8);
                }
                else
                {
                    ihevcd_parse_user_data_registered_itu_t_t35(ps_codec,
                                                                u4_payload_size);
                }
                 break;
 
             default:
 for(i = 0; i < u4_payload_size; i++)
 {
                    ihevcd_bits_flush(ps_bitstrm, 8);
 }
 break;
 }
 }

 /**
     * By definition the underlying bitstream terminates in a byte-aligned manner.
     * 1. Extract all bar the last MIN(bitsremaining,nine) bits as reserved_payload_extension_data
     * 2. Examine the final 8 bits to determine the payload_bit_equal_to_one marker
     * 3. Extract the remainingreserved_payload_extension_data bits.
     *
     * If there are fewer than 9 bits available, extract them.
     */

    payload_bits_remaining = ihevcd_bits_num_bits_remaining(ps_bitstrm);
 if(payload_bits_remaining) /* more_data_in_payload() */
 {
        WORD32 final_bits;
        WORD32 final_payload_bits = 0;
        WORD32 mask = 0xFF;
        UWORD32 u4_dummy;
        UWORD32 u4_reserved_payload_extension_data;
        UNUSED(u4_dummy);
        UNUSED(u4_reserved_payload_extension_data);

 while(payload_bits_remaining > 9)
 {
            BITS_PARSE("reserved_payload_extension_data",
                       u4_reserved_payload_extension_data, ps_bitstrm, 1);
            payload_bits_remaining--;
 }

        final_bits = ihevcd_bits_nxt(ps_bitstrm, payload_bits_remaining);

 while(final_bits & (mask >> final_payload_bits))
 {
            final_payload_bits++;
 continue;
 }

 while(payload_bits_remaining > (9 - final_payload_bits))
 {
            BITS_PARSE("reserved_payload_extension_data",
                       u4_reserved_payload_extension_data, ps_bitstrm, 1);
            payload_bits_remaining--;
 }

        BITS_PARSE("payload_bit_equal_to_one", u4_dummy, ps_bitstrm, 1);
        payload_bits_remaining--;
 while(payload_bits_remaining)
 {
            BITS_PARSE("payload_bit_equal_to_zero", u4_dummy, ps_bitstrm, 1);
            payload_bits_remaining--;
 }
 }

 return;
}
