WORD32 ih264d_parse_sei_message(dec_struct_t *ps_dec,
 dec_bit_stream_t *ps_bitstrm)
{
    UWORD32 ui4_payload_type, ui4_payload_size;
    UWORD32 u4_bits;
    WORD32 i4_status = 0;

 do
 {

         ui4_payload_type = 0;
 
         u4_bits = ih264d_get_bits_h264(ps_bitstrm, 8);
        while(0xff == u4_bits && !EXCEED_OFFSET(ps_bitstrm))
         {
             u4_bits = ih264d_get_bits_h264(ps_bitstrm, 8);
             ui4_payload_type += 255;
 }
        ui4_payload_type += u4_bits;

 
         ui4_payload_size = 0;
         u4_bits = ih264d_get_bits_h264(ps_bitstrm, 8);
        while(0xff == u4_bits && !EXCEED_OFFSET(ps_bitstrm))
         {
             u4_bits = ih264d_get_bits_h264(ps_bitstrm, 8);
             ui4_payload_size += 255;
 }
        ui4_payload_size += u4_bits;

        i4_status = ih264d_parse_sei_payload(ps_bitstrm, ui4_payload_type,
                                             ui4_payload_size, ps_dec);
 if(i4_status == -1)
 {
            i4_status = 0;
 break;
 }

 if(i4_status != OK)
 return i4_status;

 if(ih264d_check_byte_aligned(ps_bitstrm) == 0)
 {
            u4_bits = ih264d_get_bit_h264(ps_bitstrm);
 if(0 == u4_bits)

             {
                 H264_DEC_DEBUG_PRINT("\nError in parsing SEI message");
             }
            while(0 == ih264d_check_byte_aligned(ps_bitstrm)
                            && !EXCEED_OFFSET(ps_bitstrm))
             {
                 u4_bits = ih264d_get_bit_h264(ps_bitstrm);
                 if(u4_bits)
 {
                    H264_DEC_DEBUG_PRINT("\nError in parsing SEI message");
 }
 }
 }
 }
 while(ps_bitstrm->u4_ofst < ps_bitstrm->u4_max_ofst);
 return (i4_status);
}
