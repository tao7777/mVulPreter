 int main(int argc, char **argv) {
   FILE *infile = NULL;
  vpx_codec_ctx_t codec = {0};
  vpx_codec_enc_cfg_t cfg = {0};
   int frame_count = 0;
  vpx_image_t raw = {0};
   vpx_codec_err_t res;
  VpxVideoInfo info = {0};
   VpxVideoWriter *writer = NULL;
   const VpxInterface *encoder = NULL;
   const int fps = 2;        // TODO(dkovalev) add command line argument
   const double bits_per_pixel_per_frame = 0.067;
 
   exec_name = argv[0];
   if (argc != 6)
     die("Invalid number of arguments");
 
  encoder = get_vpx_encoder_by_name(argv[1]);
  if (!encoder)
    die("Unsupported codec.");
 
   info.codec_fourcc = encoder->fourcc;
   info.frame_width = strtol(argv[2], NULL, 0);
   info.frame_height = strtol(argv[3], NULL, 0);
  info.time_base.numerator = 1;
  info.time_base.denominator = fps;

 if (info.frame_width <= 0 ||
      info.frame_height <= 0 ||
 (info.frame_width % 2) != 0 ||
 (info.frame_height % 2) != 0) {
    die("Invalid frame size: %dx%d", info.frame_width, info.frame_height);
 }

 if (!vpx_img_alloc(&raw, VPX_IMG_FMT_I420, info.frame_width,
                                             info.frame_height, 1)) {

     die("Failed to allocate image.");
   }
 
  printf("Using %s\n", vpx_codec_iface_name(encoder->interface()));
 
  res = vpx_codec_enc_config_default(encoder->interface(), &cfg, 0);
   if (res)
     die_codec(&codec, "Failed to get default codec config.");
 
  cfg.g_w = info.frame_width;
  cfg.g_h = info.frame_height;
  cfg.g_timebase.num = info.time_base.numerator;
  cfg.g_timebase.den = info.time_base.denominator;
  cfg.rc_target_bitrate = (unsigned int)(bits_per_pixel_per_frame * cfg.g_w *
                                         cfg.g_h * fps / 1000);
  cfg.g_lag_in_frames = 0;

  writer = vpx_video_writer_open(argv[5], kContainerIVF, &info);
 if (!writer)
    die("Failed to open %s for writing.", argv[5]);


   if (!(infile = fopen(argv[4], "rb")))
     die("Failed to open %s for reading.", argv[4]);
 
  if (vpx_codec_enc_init(&codec, encoder->interface(), &cfg, 0))
     die_codec(&codec, "Failed to initialize encoder");
 
   while (vpx_img_read(&raw, infile)) {
     ++frame_count;
 
 if (frame_count == 22 && encoder->fourcc == VP8_FOURCC) {
      set_roi_map(&cfg, &codec);
 } else if (frame_count == 33) {
      set_active_map(&cfg, &codec);
 } else if (frame_count == 44) {
      unset_active_map(&cfg, &codec);
 }

 
     encode_frame(&codec, &raw, frame_count, writer);
   }
  encode_frame(&codec, NULL, -1, writer);
   printf("\n");
   fclose(infile);
   printf("Processed %d frames.\n", frame_count);

  vpx_img_free(&raw);
 if (vpx_codec_destroy(&codec))
    die_codec(&codec, "Failed to destroy codec.");

  vpx_video_writer_close(writer);

 return EXIT_SUCCESS;
}
