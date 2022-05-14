 string DecodeFile(const string& filename, int num_threads) {
   libvpx_test::WebMVideoSource video(filename);
   video.Init();
 
  vpx_codec_dec_cfg_t cfg = vpx_codec_dec_cfg_t();
   cfg.threads = num_threads;
   libvpx_test::VP9Decoder decoder(cfg, 0);
 
  libvpx_test::MD5 md5;
 for (video.Begin(); video.cxdata(); video.Next()) {
 const vpx_codec_err_t res =
        decoder.DecodeFrame(video.cxdata(), video.frame_size());
 if (res != VPX_CODEC_OK) {
      EXPECT_EQ(VPX_CODEC_OK, res) << decoder.DecodeError();
 break;
 }

    libvpx_test::DxDataIterator dec_iter = decoder.GetDxData();
 const vpx_image_t *img = NULL;

 while ((img = dec_iter.Next())) {
      md5.Add(img);
 }
 }

   return string(md5.Get());
 }
