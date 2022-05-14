  virtual void FramePktHook(const vpx_codec_cx_pkt_t *pkt) {
 #if WRITE_COMPRESSED_STREAM
  virtual void FramePktHook(const vpx_codec_cx_pkt_t *pkt) {
     ++out_frames_;
 
 if (pkt->data.frame.pts == 0)
      write_ivf_file_header(&cfg_, 0, outfile_);


     write_ivf_frame_header(pkt, outfile_);
     (void)fwrite(pkt->data.frame.buf, 1, pkt->data.frame.sz, outfile_);
   }
