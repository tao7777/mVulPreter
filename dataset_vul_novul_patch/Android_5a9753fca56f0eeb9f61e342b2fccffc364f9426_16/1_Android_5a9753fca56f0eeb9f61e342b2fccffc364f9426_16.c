static void encode_frame(vpx_codec_ctx_t *ctx,
                         const vpx_image_t *img,
                         vpx_codec_pts_t pts,
                         unsigned int duration,
                         vpx_enc_frame_flags_t flags,
                         unsigned int deadline,
                         VpxVideoWriter *writer) {
   vpx_codec_iter_t iter = NULL;
   const vpx_codec_cx_pkt_t *pkt = NULL;
   const vpx_codec_err_t res = vpx_codec_encode(ctx, img, pts, duration, flags,
                                               deadline);
 if (res != VPX_CODEC_OK)

     die_codec(ctx, "Failed to encode frame.");
 
   while ((pkt = vpx_codec_get_cx_data(ctx, &iter)) != NULL) {
     if (pkt->kind == VPX_CODEC_CX_FRAME_PKT) {
       const int keyframe = (pkt->data.frame.flags & VPX_FRAME_IS_KEY) != 0;
 
 if (!vpx_video_writer_write_frame(writer, pkt->data.frame.buf,
                                                pkt->data.frame.sz,
                                                pkt->data.frame.pts))
        die_codec(ctx, "Failed to write compressed frame.");
      printf(keyframe ? "K" : ".");

       fflush(stdout);
     }
   }
 }
