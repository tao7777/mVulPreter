  virtual void FramePktHook(const vpx_codec_cx_pkt_t *pkt) {
    if (pkt->data.frame.flags & VPX_FRAME_IS_KEY) {
    }
  }
