 int main(int argc, char **argv) {
   FILE *infile = NULL;
  VpxVideoWriter *writer = NULL;
   vpx_codec_ctx_t codec;
   vpx_codec_enc_cfg_t cfg;
   vpx_image_t raw;
   vpx_codec_err_t res;
  vpx_fixed_buf_t stats = {0};
  VpxVideoInfo info = {0};
   const VpxInterface *encoder = NULL;
  int pass;
   const int fps = 30;        // TODO(dkovalev) add command line argument
   const int bitrate = 200;   // kbit/s TODO(dkovalev) add command line argument
   const char *const codec_arg = argv[1];
 const char *const width_arg = argv[2];
 const char *const height_arg = argv[3];
 const char *const infile_arg = argv[4];
 const char *const outfile_arg = argv[5];
  exec_name = argv[0];

 if (argc != 6)
    die("Invalid number of arguments.");

  encoder = get_vpx_encoder_by_name(codec_arg);

   if (!encoder)
     die("Unsupported codec.");
 
  info.codec_fourcc = encoder->fourcc;
  info.time_base.numerator = 1;
  info.time_base.denominator = fps;
  info.frame_width = strtol(width_arg, NULL, 0);
  info.frame_height = strtol(height_arg, NULL, 0);
 
  if (info.frame_width <= 0 ||
      info.frame_height <= 0 ||
      (info.frame_width % 2) != 0 ||
      (info.frame_height % 2) != 0) {
    die("Invalid frame size: %dx%d", info.frame_width, info.frame_height);
  }
 
  if (!vpx_img_alloc(&raw, VPX_IMG_FMT_I420, info.frame_width,
                                             info.frame_height, 1)) {
    die("Failed to allocate image", info.frame_width, info.frame_height);
  }
 
  writer = vpx_video_writer_open(outfile_arg, kContainerIVF, &info);
  if (!writer)
    die("Failed to open %s for writing", outfile_arg);
 
  printf("Using %s\n", vpx_codec_iface_name(encoder->interface()));
  res = vpx_codec_enc_config_default(encoder->interface(), &cfg, 0);
   if (res)
     die_codec(&codec, "Failed to get default codec config.");
 
  cfg.g_w = info.frame_width;
  cfg.g_h = info.frame_height;
  cfg.g_timebase.num = info.time_base.numerator;
  cfg.g_timebase.den = info.time_base.denominator;
   cfg.rc_target_bitrate = bitrate;
 
  for (pass = 0; pass < 2; ++pass) {
    int frame_count = 0;
 
    if (pass == 0) {
      cfg.g_pass = VPX_RC_FIRST_PASS;
    } else {
      cfg.g_pass = VPX_RC_LAST_PASS;
      cfg.rc_twopass_stats_in = stats;
    }
 
    if (!(infile = fopen(infile_arg, "rb")))
      die("Failed to open %s for reading", infile_arg);
    if (vpx_codec_enc_init(&codec, encoder->interface(), &cfg, 0))
      die_codec(&codec, "Failed to initialize encoder");
    while (vpx_img_read(&raw, infile)) {
      ++frame_count;
      if (pass == 0) {
        get_frame_stats(&codec, &raw, frame_count, 1, 0, VPX_DL_BEST_QUALITY,
                        &stats);
      } else {
        encode_frame(&codec, &raw, frame_count, 1, 0, VPX_DL_BEST_QUALITY,
                     writer);
      }
    }
    if (pass == 0) {
      get_frame_stats(&codec, NULL, frame_count, 1, 0, VPX_DL_BEST_QUALITY,
                      &stats);
    } else {
      printf("\n");
    }
    fclose(infile);
    printf("Pass %d complete. Processed %d frames.\n", pass + 1, frame_count);
    if (vpx_codec_destroy(&codec))
      die_codec(&codec, "Failed to destroy codec.");
  }
  vpx_img_free(&raw);
   free(stats.buf);
 
  vpx_video_writer_close(writer);
 
   return EXIT_SUCCESS;
 }
