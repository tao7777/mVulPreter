standard_display_init(standard_display *dp, png_store* ps, png_uint_32 id,
 int do_interlace, int use_update_info)
{
   memset(dp, 0, sizeof *dp);

   dp->ps = ps;
   dp->colour_type = COL_FROM_ID(id);
   dp->bit_depth = DEPTH_FROM_ID(id);
 if (dp->bit_depth < 1 || dp->bit_depth > 16)
      internal_error(ps, "internal: bad bit depth");
 if (dp->colour_type == 3)
      dp->red_sBIT = dp->blue_sBIT = dp->green_sBIT = dp->alpha_sBIT = 8;
 else
      dp->red_sBIT = dp->blue_sBIT = dp->green_sBIT = dp->alpha_sBIT =
         dp->bit_depth;
   dp->interlace_type = INTERLACE_FROM_ID(id);
   check_interlace_type(dp->interlace_type);
   dp->id = id;
 /* All the rest are filled in after the read_info: */
   dp->w = 0;
   dp->h = 0;
   dp->npasses = 0;
   dp->pixel_size = 0;

    dp->bit_width = 0;
    dp->cbRow = 0;
    dp->do_interlace = do_interlace;
    dp->is_transparent = 0;
    dp->speed = ps->speed;
    dp->use_update_info = use_update_info;
   dp->npalette = 0;
 /* Preset the transparent color to black: */
   memset(&dp->transparent, 0, sizeof dp->transparent);
 /* Preset the palette to full intensity/opaque througout: */
   memset(dp->palette, 0xff, sizeof dp->palette);
}
