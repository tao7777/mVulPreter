 vpx_codec_err_t Decoder::DecodeFrame(const uint8_t *cxdata, size_t size) {
   vpx_codec_err_t res_dec;
   InitOnce();
  REGISTER_STATE_CHECK(
       res_dec = vpx_codec_decode(&decoder_,
                                  cxdata, static_cast<unsigned int>(size),
                                 NULL, 0));
   return res_dec;
 }
