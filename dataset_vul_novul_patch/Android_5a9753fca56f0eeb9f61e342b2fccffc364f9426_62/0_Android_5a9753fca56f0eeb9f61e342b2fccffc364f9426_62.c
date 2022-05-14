 void Encoder::Flush() {
   const vpx_codec_err_t res = vpx_codec_encode(&encoder_, NULL, 0, 0, 0,
                                                deadline_);
  if (!encoder_.priv)
    ASSERT_EQ(VPX_CODEC_ERROR, res) << EncoderError();
  else
    ASSERT_EQ(VPX_CODEC_OK, res) << EncoderError();
 }
