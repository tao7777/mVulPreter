read_gif(Gif_Reader *grr, int read_flags,
	 const char* landmark, Gif_ReadErrorHandler handler)
{
  Gif_Stream *gfs;
  Gif_Image *gfi;
  Gif_Context gfc;
  int unknown_block_type = 0;

  if (gifgetc(grr) != 'G' ||
      gifgetc(grr) != 'I' ||
      gifgetc(grr) != 'F')
    return 0;
  (void)gifgetc(grr);
  (void)gifgetc(grr);
  (void)gifgetc(grr);

  gfs = Gif_NewStream();
  gfi = Gif_NewImage();

  gfc.stream = gfs;
  gfc.prefix = Gif_NewArray(Gif_Code, GIF_MAX_CODE);
  gfc.suffix = Gif_NewArray(uint8_t, GIF_MAX_CODE);
  gfc.length = Gif_NewArray(uint16_t, GIF_MAX_CODE);
  gfc.handler = handler;
  gfc.gfi = gfi;
  gfc.errors[0] = gfc.errors[1] = 0;

  if (!gfs || !gfi || !gfc.prefix || !gfc.suffix || !gfc.length)
    goto done;
  gfs->landmark = landmark;

  GIF_DEBUG(("\nGIF "));
  if (!read_logical_screen_descriptor(gfs, grr))
    goto done;
  GIF_DEBUG(("logscrdesc "));

  while (!gifeof(grr)) {

    uint8_t block = gifgetbyte(grr);

    switch (block) {

     case ',': /* image block */
      GIF_DEBUG(("imageread %d ", gfs->nimages));

      gfi->identifier = last_name;
      last_name = 0;
      if (!Gif_AddImage(gfs, gfi))
          goto done;
      else if (!read_image(grr, &gfc, gfi, read_flags)) {
          Gif_RemoveImage(gfs, gfs->nimages - 1);
          gfi = 0;
          goto done;
      }

      gfc.gfi = gfi = Gif_NewImage();
      if (!gfi)
          goto done;
      break;

     case ';': /* terminator */
      GIF_DEBUG(("term\n"));
      goto done;

     case '!': /* extension */
      block = gifgetbyte(grr);
      GIF_DEBUG(("ext(0x%02X) ", block));
      switch (block) {

       case 0xF9:
	read_graphic_control_extension(&gfc, gfi, grr);
	break;

       case 0xCE:
	last_name = suck_data(last_name, 0, grr);
	break;

       case 0xFE:
	if (!read_comment_extension(gfi, grr)) goto done;
	break;

       case 0xFF:
	read_application_extension(&gfc, grr);
	break;

       default:
        read_unknown_extension(&gfc, grr, block, 0, 0);
	break;

      }
      break;

     default:
       if (!unknown_block_type) {
	 char buf[256];
	 sprintf(buf, "unknown block type %d at file offset %u", block, grr->pos - 1);
	 gif_read_error(&gfc, 1, buf);
	 unknown_block_type = 1;
       }
       break;

    }

  }

 done:

  /* Move comments and extensions after last image into stream. */
  if (gfs && gfi) {
      Gif_Extension* gfex;
      gfs->end_comment = gfi->comment;
      gfi->comment = 0;
      gfs->end_extension_list = gfi->extension_list;
      gfi->extension_list = 0;
      for (gfex = gfs->end_extension_list; gfex; gfex = gfex->next)
          gfex->image = NULL;
  }

  Gif_DeleteImage(gfi);
  Gif_DeleteArray(last_name);
  Gif_DeleteArray(gfc.prefix);
   Gif_DeleteArray(gfc.suffix);
   Gif_DeleteArray(gfc.length);
   gfc.gfi = 0;
  last_name = 0;
 
   if (gfs)
     gfs->errors = gfc.errors[1];
  if (gfs && gfc.errors[1] == 0
      && !(read_flags & GIF_READ_TRAILING_GARBAGE_OK)
      && !grr->eofer(grr))
      gif_read_error(&gfc, 0, "trailing garbage after GIF ignored");
  /* finally, export last message */
  gif_read_error(&gfc, -1, 0);

  return gfs;
}
