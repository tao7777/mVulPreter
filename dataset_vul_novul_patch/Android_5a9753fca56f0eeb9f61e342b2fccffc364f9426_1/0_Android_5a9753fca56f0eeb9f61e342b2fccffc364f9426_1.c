int main(int argc, char **argv) {
 int frame_cnt = 0;
 FILE *outfile = NULL;
 vpx_codec_ctx_t codec;
 const VpxInterface *decoder = NULL;
 VpxVideoReader *reader = NULL;
 const VpxVideoInfo *info = NULL;
 int n = 0;
 int m = 0;
 int is_range = 0;
 char *nptr = NULL;

  exec_name = argv[0];

 if (argc != 4)
    die("Invalid number of arguments.");

  reader = vpx_video_reader_open(argv[1]);
 if (!reader)
    die("Failed to open %s for reading.", argv[1]);

 if (!(outfile = fopen(argv[2], "wb")))
    die("Failed to open %s for writing.", argv[2]);

  n = strtol(argv[3], &nptr, 0);
  m = strtol(nptr + 1, NULL, 0);
  is_range = (*nptr == '-');
 if (!n || !m || (*nptr != '-' && *nptr != '/'))
    die("Couldn't parse pattern %s.\n", argv[3]);

  info = vpx_video_reader_get_info(reader);

  decoder = get_vpx_decoder_by_fourcc(info->codec_fourcc);

   if (!decoder)
     die("Unknown input codec.");
 
  printf("Using %s\n", vpx_codec_iface_name(decoder->codec_interface()));
 
  if (vpx_codec_dec_init(&codec, decoder->codec_interface(), NULL, 0))
     die_codec(&codec, "Failed to initialize decoder.");
 
   while (vpx_video_reader_read_frame(reader)) {
 vpx_codec_iter_t iter = NULL;
 vpx_image_t *img = NULL;
 size_t frame_size = 0;
 int skip;
 const unsigned char *frame = vpx_video_reader_get_frame(reader,
 &frame_size);
 if (vpx_codec_decode(&codec, frame, (unsigned int)frame_size, NULL, 0))
      die_codec(&codec, "Failed to decode frame.");

 ++frame_cnt;

    skip = (is_range && frame_cnt >= n && frame_cnt <= m) ||
 (!is_range && m - (frame_cnt - 1) % m <= n);

 if (!skip) {
      putc('.', stdout);

 while ((img = vpx_codec_get_frame(&codec, &iter)) != NULL)
        vpx_img_write(img, outfile);
 } else {
      putc('X', stdout);
 }

    fflush(stdout);
 }

  printf("Processed %d frames.\n", frame_cnt);
 if (vpx_codec_destroy(&codec))
    die_codec(&codec, "Failed to destroy codec.");

  printf("Play: ffplay -f rawvideo -pix_fmt yuv420p -s %dx%d %s\n",
         info->frame_width, info->frame_height, argv[2]);

  vpx_video_reader_close(reader);
  fclose(outfile);

 return EXIT_SUCCESS;
}
