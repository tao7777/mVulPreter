 int main(int argc, char **argv) {
   FILE *infile = NULL;
  int w, h;
   vpx_codec_ctx_t codec;
   vpx_codec_enc_cfg_t cfg;
   vpx_image_t raw;
   vpx_codec_err_t res;
  vpx_fixed_buf_t stats;

   const VpxInterface *encoder = NULL;
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
 
  w = strtol(width_arg, NULL, 0);
  h = strtol(height_arg, NULL, 0);
 
  if (w  <= 0 || h <= 0 || (w % 2) != 0 || (h  % 2) != 0)
    die("Invalid frame size: %dx%d", w, h);
 
  if (!vpx_img_alloc(&raw, VPX_IMG_FMT_I420, w, h, 1))
    die("Failed to allocate image", w, h);
 
  printf("Using %s\n", vpx_codec_iface_name(encoder->codec_interface()));
 
  // Configuration
  res = vpx_codec_enc_config_default(encoder->codec_interface(), &cfg, 0);
   if (res)
     die_codec(&codec, "Failed to get default codec config.");
 
  cfg.g_w = w;
  cfg.g_h = h;
  cfg.g_timebase.num = 1;
  cfg.g_timebase.den = fps;
   cfg.rc_target_bitrate = bitrate;
 
  if (!(infile = fopen(infile_arg, "rb")))
    die("Failed to open %s for reading", infile_arg);
 
  // Pass 0
  cfg.g_pass = VPX_RC_FIRST_PASS;
  stats = pass0(&raw, infile, encoder, &cfg);
 
  // Pass 1
  rewind(infile);
  cfg.g_pass = VPX_RC_LAST_PASS;
  cfg.rc_twopass_stats_in = stats;
  pass1(&raw, infile, outfile_arg, encoder, &cfg);
   free(stats.buf);
 
  vpx_img_free(&raw);
  fclose(infile);
 
   return EXIT_SUCCESS;
 }
