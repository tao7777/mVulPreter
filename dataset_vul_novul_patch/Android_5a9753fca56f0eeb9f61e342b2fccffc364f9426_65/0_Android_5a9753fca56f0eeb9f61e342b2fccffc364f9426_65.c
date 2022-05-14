 void EncoderTest::RunLoop(VideoSource *video) {
  vpx_codec_dec_cfg_t dec_cfg = vpx_codec_dec_cfg_t();
 
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

    video->Begin();
    encoder->InitEncoder(video);

    unsigned long dec_init_flags = 0;  // NOLINT
    // Use fragment decoder if encoder outputs partitions.
    // NOTE: fragment decoder and partition encoder are only supported by VP8.
    if (init_flags_ & VPX_CODEC_USE_OUTPUT_PARTITION)
      dec_init_flags |= VPX_CODEC_USE_INPUT_FRAGMENTS;
    Decoder* const decoder = codec_->CreateDecoder(dec_cfg, dec_init_flags, 0);
     bool again;
    for (again = true; again; video->Next()) {
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

              if (!HandleDecodeResult(res_dec, *video, decoder))
                break;

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
 
      // Flush the decoder when there are no more fragments.
      if ((init_flags_ & VPX_CODEC_USE_OUTPUT_PARTITION) && has_dxdata) {
        const vpx_codec_err_t res_dec = decoder->DecodeFrame(NULL, 0);
        if (!HandleDecodeResult(res_dec, *video, decoder))
          break;
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
