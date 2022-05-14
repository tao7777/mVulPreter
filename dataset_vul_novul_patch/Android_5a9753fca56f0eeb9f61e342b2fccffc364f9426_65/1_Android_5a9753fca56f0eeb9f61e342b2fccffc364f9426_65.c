 void EncoderTest::RunLoop(VideoSource *video) {
  vpx_codec_dec_cfg_t dec_cfg = {0};
 
   stats_.Reset();
 
  ASSERT_TRUE(passes_ == 1 || passes_ == 2);
 for (unsigned int pass = 0; pass < passes_; pass++) {
    last_pts_ = 0;

 if (passes_ == 1)
      cfg_.g_pass = VPX_RC_ONE_PASS;
 else if (pass == 0)
      cfg_.g_pass = VPX_RC_FIRST_PASS;
 else
      cfg_.g_pass = VPX_RC_LAST_PASS;

 BeginPassHook(pass);

     Encoder* const encoder = codec_->CreateEncoder(cfg_, deadline_, init_flags_,
                                                    &stats_);
     ASSERT_TRUE(encoder != NULL);
    Decoder* const decoder = codec_->CreateDecoder(dec_cfg, 0);
     bool again;
    for (again = true, video->Begin(); again; video->Next()) {
       again = (video->img() != NULL);
 
       PreEncodeFrameHook(video);
 PreEncodeFrameHook(video, encoder);
      encoder->EncodeFrame(video, frame_flags_);

 CxDataIterator iter = encoder->GetCxData();

 bool has_cxdata = false;
 bool has_dxdata = false;
 while (const vpx_codec_cx_pkt_t *pkt = iter.Next()) {
        pkt = MutateEncoderOutputHook(pkt);
        again = true;
 switch (pkt->kind) {
 case VPX_CODEC_CX_FRAME_PKT:
            has_cxdata = true;

             if (decoder && DoDecode()) {
               vpx_codec_err_t res_dec = decoder->DecodeFrame(
                   (const uint8_t*)pkt->data.frame.buf, pkt->data.frame.sz);
              ASSERT_EQ(VPX_CODEC_OK, res_dec) << decoder->DecodeError();
               has_dxdata = true;
             }
             ASSERT_GE(pkt->data.frame.pts, last_pts_);
            last_pts_ = pkt->data.frame.pts;
 FramePktHook(pkt);
 break;

 case VPX_CODEC_PSNR_PKT:
 PSNRPktHook(pkt);
 break;

 default:
 break;

         }
       }
 
       if (has_dxdata && has_cxdata) {
         const vpx_image_t *img_enc = encoder->GetPreviewFrame();
         DxDataIterator dec_iter = decoder->GetDxData();
 const vpx_image_t *img_dec = dec_iter.Next();
 if (img_enc && img_dec) {
 const bool res = compare_img(img_enc, img_dec);
 if (!res) { // Mismatch
 MismatchHook(img_enc, img_dec);
 }
 }
 if (img_dec)
 DecompressedFrameHook(*img_dec, video->pts());
 }
 if (!Continue())
 break;
 }

 EndPassHook();

 if (decoder)
 delete decoder;
 delete encoder;

 if (!Continue())
 break;
 }
}
