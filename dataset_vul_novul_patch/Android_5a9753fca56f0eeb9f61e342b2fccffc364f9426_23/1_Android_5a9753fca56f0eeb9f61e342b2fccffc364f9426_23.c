 int main(int argc, char **argv) {
 FILE *infile = NULL;
 vpx_codec_ctx_t codec = {0};
 vpx_codec_enc_cfg_t cfg = {0};
 int frame_count = 0;
 vpx_image_t raw;
 vpx_codec_err_t res;
 VpxVideoInfo info = {0};
 VpxVideoWriter *writer = NULL;
 const VpxInterface *encoder = NULL;
 int update_frame_num = 0;
 const int fps = 30; // TODO(dkovalev) add command line argument
 const int bitrate = 200; // kbit/s TODO(dkovalev) add command line argument

  exec_name = argv[0];

 if (argc != 6)
    die("Invalid number of arguments");

  encoder = get_vpx_encoder_by_name("vp8");
 if (!encoder)
    die("Unsupported codec.");

  update_frame_num = atoi(argv[5]);
 if (!update_frame_num)
    die("Couldn't parse frame number '%s'\n", argv[5]);

  info.codec_fourcc = encoder->fourcc;
  info.frame_width = strtol(argv[1], NULL, 0);
  info.frame_height = strtol(argv[2], NULL, 0);
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
  cfg.rc_target_bitrate = bitrate;

  writer = vpx_video_writer_open(argv[4], kContainerIVF, &info);
 if (!writer)
    die("Failed to open %s for writing.", argv[4]);


   if (!(infile = fopen(argv[3], "rb")))
     die("Failed to open %s for reading.", argv[3]);
 
  if (vpx_codec_enc_init(&codec, encoder->interface(), &cfg, 0))
     die_codec(&codec, "Failed to initialize encoder");
 
   while (vpx_img_read(&raw, infile)) {
     if (frame_count + 1 == update_frame_num) {
       vpx_ref_frame_t ref;
      ref.frame_type = VP8_LAST_FRAME;
      ref.img = raw;
 if (vpx_codec_control(&codec, VP8_SET_REFERENCE, &ref))
        die_codec(&codec, "Failed to set reference frame");
 }

 
     encode_frame(&codec, &raw, frame_count++, writer);
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
