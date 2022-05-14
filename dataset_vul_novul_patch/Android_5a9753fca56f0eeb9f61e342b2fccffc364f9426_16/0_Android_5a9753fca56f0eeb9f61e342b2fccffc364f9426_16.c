static void encode_frame(vpx_codec_ctx_t *ctx,
static int encode_frame(vpx_codec_ctx_t *ctx,
                        const vpx_image_t *img,
                        vpx_codec_pts_t pts,
                        unsigned int duration,
                        vpx_enc_frame_flags_t flags,
                        unsigned int deadline,
                        VpxVideoWriter *writer) {
  int got_pkts = 0;
   vpx_codec_iter_t iter = NULL;
   const vpx_codec_cx_pkt_t *pkt = NULL;
   const vpx_codec_err_t res = vpx_codec_encode(ctx, img, pts, duration, flags,
                                               deadline);
 if (res != VPX_CODEC_OK)

     die_codec(ctx, "Failed to encode frame.");
 
   while ((pkt = vpx_codec_get_cx_data(ctx, &iter)) != NULL) {
    got_pkts = 1;
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

  return got_pkts;
}

static vpx_fixed_buf_t pass0(vpx_image_t *raw,
                             FILE *infile,
                             const VpxInterface *encoder,
                             const vpx_codec_enc_cfg_t *cfg) {
  vpx_codec_ctx_t codec;
  int frame_count = 0;
  vpx_fixed_buf_t stats = {NULL, 0};

  if (vpx_codec_enc_init(&codec, encoder->codec_interface(), cfg, 0))
    die_codec(&codec, "Failed to initialize encoder");

  // Calculate frame statistics.
  while (vpx_img_read(raw, infile)) {
    ++frame_count;
    get_frame_stats(&codec, raw, frame_count, 1, 0, VPX_DL_GOOD_QUALITY,
                    &stats);
  }

  // Flush encoder.
  while (get_frame_stats(&codec, NULL, frame_count, 1, 0,
                         VPX_DL_GOOD_QUALITY, &stats)) {}

  printf("Pass 0 complete. Processed %d frames.\n", frame_count);
  if (vpx_codec_destroy(&codec))
    die_codec(&codec, "Failed to destroy codec.");

  return stats;
}

static void pass1(vpx_image_t *raw,
                  FILE *infile,
                  const char *outfile_name,
                  const VpxInterface *encoder,
                  const vpx_codec_enc_cfg_t *cfg) {
  VpxVideoInfo info = {
    encoder->fourcc,
    cfg->g_w,
    cfg->g_h,
    {cfg->g_timebase.num, cfg->g_timebase.den}
  };
  VpxVideoWriter *writer = NULL;
  vpx_codec_ctx_t codec;
  int frame_count = 0;

  writer = vpx_video_writer_open(outfile_name, kContainerIVF, &info);
  if (!writer)
    die("Failed to open %s for writing", outfile_name);

  if (vpx_codec_enc_init(&codec, encoder->codec_interface(), cfg, 0))
    die_codec(&codec, "Failed to initialize encoder");

  // Encode frames.
  while (vpx_img_read(raw, infile)) {
    ++frame_count;
    encode_frame(&codec, raw, frame_count, 1, 0, VPX_DL_GOOD_QUALITY, writer);
  }

  // Flush encoder.
  while (encode_frame(&codec, NULL, -1, 1, 0, VPX_DL_GOOD_QUALITY, writer)) {}

  printf("\n");

  if (vpx_codec_destroy(&codec))
    die_codec(&codec, "Failed to destroy codec.");

  vpx_video_writer_close(writer);

  printf("Pass 1 complete. Processed %d frames.\n", frame_count);
 }
