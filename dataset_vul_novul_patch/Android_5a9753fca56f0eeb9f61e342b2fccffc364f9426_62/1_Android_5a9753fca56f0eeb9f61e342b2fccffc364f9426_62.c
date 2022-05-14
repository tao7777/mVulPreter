 void Encoder::Flush() {
   const vpx_codec_err_t res = vpx_codec_encode(&encoder_, NULL, 0, 0, 0,
                                                deadline_);
  ASSERT_EQ(VPX_CODEC_OK, res) << EncoderError();
 }
