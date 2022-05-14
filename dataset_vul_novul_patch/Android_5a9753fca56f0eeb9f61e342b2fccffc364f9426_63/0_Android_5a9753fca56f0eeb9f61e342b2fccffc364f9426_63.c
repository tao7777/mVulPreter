 void EncoderTest::InitializeConfig() {
   const vpx_codec_err_t res = codec_->DefaultEncoderConfig(&cfg_, 0);
  dec_cfg_ = vpx_codec_dec_cfg_t();
   ASSERT_EQ(VPX_CODEC_OK, res);
 }
