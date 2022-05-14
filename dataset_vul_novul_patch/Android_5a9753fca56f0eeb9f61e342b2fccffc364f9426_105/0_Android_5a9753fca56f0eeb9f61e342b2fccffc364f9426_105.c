   virtual void SetUp() {
     svc_.log_level = SVC_LOG_DEBUG;
     svc_.log_print = 0;
 
    codec_iface_ = vpx_codec_vp9_cx();
 const vpx_codec_err_t res =
        vpx_codec_enc_config_default(codec_iface_, &codec_enc_, 0);
    EXPECT_EQ(VPX_CODEC_OK, res);

    codec_enc_.g_w = kWidth;
    codec_enc_.g_h = kHeight;
    codec_enc_.g_timebase.num = 1;
    codec_enc_.g_timebase.den = 60;

     codec_enc_.kf_min_dist = 100;
     codec_enc_.kf_max_dist = 100;
 
    vpx_codec_dec_cfg_t dec_cfg = vpx_codec_dec_cfg_t();
     VP9CodecFactory codec_factory;
     decoder_ = codec_factory.CreateDecoder(dec_cfg, 0);

    tile_columns_ = 0;
    tile_rows_ = 0;
   }
