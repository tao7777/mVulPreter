 void EncoderTest::InitializeConfig() {
   const vpx_codec_err_t res = codec_->DefaultEncoderConfig(&cfg_, 0);
   ASSERT_EQ(VPX_CODEC_OK, res);
 }
