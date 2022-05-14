static void get_frame_stats(vpx_codec_ctx_t *ctx,
static int get_frame_stats(vpx_codec_ctx_t *ctx,
                           const vpx_image_t *img,
                           vpx_codec_pts_t pts,
                           unsigned int duration,
                           vpx_enc_frame_flags_t flags,
                           unsigned int deadline,
                           vpx_fixed_buf_t *stats) {
  int got_pkts = 0;
   vpx_codec_iter_t iter = NULL;
   const vpx_codec_cx_pkt_t *pkt = NULL;
   const vpx_codec_err_t res = vpx_codec_encode(ctx, img, pts, duration, flags,
                                               deadline);
 if (res != VPX_CODEC_OK)

     die_codec(ctx, "Failed to get frame stats.");
 
   while ((pkt = vpx_codec_get_cx_data(ctx, &iter)) != NULL) {
    got_pkts = 1;

     if (pkt->kind == VPX_CODEC_STATS_PKT) {
       const uint8_t *const pkt_buf = pkt->data.twopass_stats.buf;
       const size_t pkt_size = pkt->data.twopass_stats.sz;
      stats->buf = realloc(stats->buf, stats->sz + pkt_size);
      memcpy((uint8_t *)stats->buf + stats->sz, pkt_buf, pkt_size);

       stats->sz += pkt_size;
     }
   }

  return got_pkts;
 }
