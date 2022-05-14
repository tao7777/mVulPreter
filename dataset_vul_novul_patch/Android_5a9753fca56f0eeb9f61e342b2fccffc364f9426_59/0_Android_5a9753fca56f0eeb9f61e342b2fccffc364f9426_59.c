 vpx_codec_err_t Decoder::DecodeFrame(const uint8_t *cxdata, size_t size) {
  return DecodeFrame(cxdata, size, NULL);
}

vpx_codec_err_t Decoder::DecodeFrame(const uint8_t *cxdata, size_t size,
                                     void *user_priv) {
   vpx_codec_err_t res_dec;
   InitOnce();
  API_REGISTER_STATE_CHECK(
       res_dec = vpx_codec_decode(&decoder_,
                                  cxdata, static_cast<unsigned int>(size),
                                 user_priv, 0));
   return res_dec;
 }
