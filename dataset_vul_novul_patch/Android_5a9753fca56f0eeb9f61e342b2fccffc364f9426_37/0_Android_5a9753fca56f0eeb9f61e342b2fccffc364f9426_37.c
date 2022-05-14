  virtual void FramePktHook(const vpx_codec_cx_pkt_t *pkt) {
  virtual void PSNRPktHook(const vpx_codec_cx_pkt_t *pkt) {
    if (pkt->data.psnr.psnr[0] < min_psnr_)
      min_psnr_ = pkt->data.psnr.psnr[0];
   }
