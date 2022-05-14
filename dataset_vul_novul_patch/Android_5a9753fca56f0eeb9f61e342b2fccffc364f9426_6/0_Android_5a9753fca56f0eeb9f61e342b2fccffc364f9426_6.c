static void encode_frame(vpx_codec_ctx_t *codec,
static int encode_frame(vpx_codec_ctx_t *codec,
                        vpx_image_t *img,
                        int frame_index,
                        VpxVideoWriter *writer) {
  int got_pkts = 0;
   vpx_codec_iter_t iter = NULL;
   const vpx_codec_cx_pkt_t *pkt = NULL;
   const vpx_codec_err_t res = vpx_codec_encode(codec, img, frame_index, 1, 0,
                                               VPX_DL_GOOD_QUALITY);
 if (res != VPX_CODEC_OK)

     die_codec(codec, "Failed to encode frame");
 
   while ((pkt = vpx_codec_get_cx_data(codec, &iter)) != NULL) {
    got_pkts = 1;

     if (pkt->kind == VPX_CODEC_CX_FRAME_PKT) {
       const int keyframe = (pkt->data.frame.flags & VPX_FRAME_IS_KEY) != 0;
       if (!vpx_video_writer_write_frame(writer,
                                        pkt->data.frame.buf,
                                        pkt->data.frame.sz,
                                        pkt->data.frame.pts)) {
        die_codec(codec, "Failed to write compressed frame");
 }

      printf(keyframe ? "K" : ".");

       fflush(stdout);
     }
   }

  return got_pkts;
 }
