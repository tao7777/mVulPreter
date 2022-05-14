void Encoder::EncodeFrameInternal(const VideoSource &video,
 const unsigned long frame_flags) {

   vpx_codec_err_t res;
   const vpx_image_t *img = video.img();
 
   if (cfg_.g_w != img->d_w || cfg_.g_h != img->d_h) {
     cfg_.g_w = img->d_w;
    cfg_.g_h = img->d_h;
    res = vpx_codec_enc_config_set(&encoder_, &cfg_);
    ASSERT_EQ(VPX_CODEC_OK, res) << EncoderError();

   }
 
  API_REGISTER_STATE_CHECK(
      res = vpx_codec_encode(&encoder_, img, video.pts(), video.duration(),
                              frame_flags, deadline_));
   ASSERT_EQ(VPX_CODEC_OK, res) << EncoderError();
 }
