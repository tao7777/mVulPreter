static vpx_codec_err_t decoder_peek_si_internal(const uint8_t *data,
 unsigned int data_sz,
 vpx_codec_stream_info_t *si,
 int *is_intra_only,

                                                 vpx_decrypt_cb decrypt_cb,
                                                 void *decrypt_state) {
   int intra_only_flag = 0;
  uint8_t clear_buffer[10];
 
   if (data + data_sz <= data)
     return VPX_CODEC_INVALID_PARAM;

  si->is_kf = 0;
  si->w = si->h = 0;

 if (decrypt_cb) {
    data_sz = VPXMIN(sizeof(clear_buffer), data_sz);
    decrypt_cb(decrypt_state, data, clear_buffer, data_sz);

     data = clear_buffer;
   }
 
  // A maximum of 6 bits are needed to read the frame marker, profile and
  // show_existing_frame.
  if (data_sz < 1)
    return VPX_CODEC_UNSUP_BITSTREAM;

   {
     int show_frame;
     int error_resilient;
 struct vpx_read_bit_buffer rb = { data, data + data_sz, 0, NULL, NULL };
 const int frame_marker = vpx_rb_read_literal(&rb, 2);
 const BITSTREAM_PROFILE profile = vp9_read_profile(&rb);

 if (frame_marker != VP9_FRAME_MARKER)
 return VPX_CODEC_UNSUP_BITSTREAM;


     if (profile >= MAX_PROFILES)
       return VPX_CODEC_UNSUP_BITSTREAM;
 
     if (vpx_rb_read_bit(&rb)) {  // show an existing frame
      // If profile is > 2 and show_existing_frame is true, then at least 1 more
      // byte (6+3=9 bits) is needed.
      if (profile > 2 && data_sz < 2)
        return VPX_CODEC_UNSUP_BITSTREAM;
       vpx_rb_read_literal(&rb, 3);  // Frame buffer to show.
       return VPX_CODEC_OK;
     }
 
    // For the rest of the function, a maximum of 9 more bytes are needed
    // (computed by taking the maximum possible bits needed in each case). Note
    // that this has to be updated if we read any more bits in this function.
    if (data_sz < 10)
       return VPX_CODEC_UNSUP_BITSTREAM;
 
     si->is_kf = !vpx_rb_read_bit(&rb);
    show_frame = vpx_rb_read_bit(&rb);
    error_resilient = vpx_rb_read_bit(&rb);

 if (si->is_kf) {
 if (!vp9_read_sync_code(&rb))
 return VPX_CODEC_UNSUP_BITSTREAM;

 if (!parse_bitdepth_colorspace_sampling(profile, &rb))
 return VPX_CODEC_UNSUP_BITSTREAM;
      vp9_read_frame_size(&rb, (int *)&si->w, (int *)&si->h);
 } else {
      intra_only_flag = show_frame ? 0 : vpx_rb_read_bit(&rb);

      rb.bit_offset += error_resilient ? 0 : 2; // reset_frame_context

 if (intra_only_flag) {
 if (!vp9_read_sync_code(&rb))
 return VPX_CODEC_UNSUP_BITSTREAM;
 if (profile > PROFILE_0) {
 if (!parse_bitdepth_colorspace_sampling(profile, &rb))
 return VPX_CODEC_UNSUP_BITSTREAM;
 }
        rb.bit_offset += REF_FRAMES; // refresh_frame_flags
        vp9_read_frame_size(&rb, (int *)&si->w, (int *)&si->h);
 }
 }
 }
 if (is_intra_only != NULL)
 *is_intra_only = intra_only_flag;
 return VPX_CODEC_OK;
}
