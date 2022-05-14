standard_info_part1(standard_display *dp, png_structp pp, png_infop pi)
{
 if (png_get_bit_depth(pp, pi) != dp->bit_depth)
      png_error(pp, "validate: bit depth changed");

 if (png_get_color_type(pp, pi) != dp->colour_type)
      png_error(pp, "validate: color type changed");

 if (png_get_filter_type(pp, pi) != PNG_FILTER_TYPE_BASE)
      png_error(pp, "validate: filter type changed");

 if (png_get_interlace_type(pp, pi) != dp->interlace_type)
      png_error(pp, "validate: interlacing changed");

 if (png_get_compression_type(pp, pi) != PNG_COMPRESSION_TYPE_BASE)
      png_error(pp, "validate: compression type changed");

   dp->w = png_get_image_width(pp, pi);

 if (dp->w != standard_width(pp, dp->id))
      png_error(pp, "validate: image width changed");

   dp->h = png_get_image_height(pp, pi);

 if (dp->h != standard_height(pp, dp->id))
      png_error(pp, "validate: image height changed");

 /* Record (but don't check at present) the input sBIT according to the colour
    * type information.
    */
 {
      png_color_8p sBIT = 0;

 if (png_get_sBIT(pp, pi, &sBIT) & PNG_INFO_sBIT)
 {
 int sBIT_invalid = 0;

 if (sBIT == 0)
            png_error(pp, "validate: unexpected png_get_sBIT result");

 if (dp->colour_type & PNG_COLOR_MASK_COLOR)
 {
 if (sBIT->red == 0 || sBIT->red > dp->bit_depth)
               sBIT_invalid = 1;
 else
               dp->red_sBIT = sBIT->red;

 if (sBIT->green == 0 || sBIT->green > dp->bit_depth)
               sBIT_invalid = 1;
 else
               dp->green_sBIT = sBIT->green;

 if (sBIT->blue == 0 || sBIT->blue > dp->bit_depth)
               sBIT_invalid = 1;
 else
               dp->blue_sBIT = sBIT->blue;
 }

 else /* !COLOR */
 {
 if (sBIT->gray == 0 || sBIT->gray > dp->bit_depth)
               sBIT_invalid = 1;
 else
               dp->blue_sBIT = dp->green_sBIT = dp->red_sBIT = sBIT->gray;
 }

 /* All 8 bits in tRNS for a palette image are significant - see the
          * spec.
          */
 if (dp->colour_type & PNG_COLOR_MASK_ALPHA)
 {
 if (sBIT->alpha == 0 || sBIT->alpha > dp->bit_depth)
               sBIT_invalid = 1;
 else
               dp->alpha_sBIT = sBIT->alpha;
 }

 if (sBIT_invalid)
            png_error(pp, "validate: sBIT value out of range");
 }
 }

 /* Important: this is validating the value *before* any transforms have been
    * put in place.  It doesn't matter for the standard tests, where there are
    * no transforms, but it does for other tests where rowbytes may change after
    * png_read_update_info.
    */
 if (png_get_rowbytes(pp, pi) != standard_rowsize(pp, dp->id))
      png_error(pp, "validate: row size changed");

 /* Validate the colour type 3 palette (this can be present on other color
    * types.)
    */
   standard_palette_validate(dp, pp, pi);

 /* In any case always check for a tranparent color (notice that the
    * colour type 3 case must not give a successful return on the get_tRNS call
    * with these arguments!)
    */
 {
      png_color_16p trans_color = 0;

 if (png_get_tRNS(pp, pi, 0, 0, &trans_color) & PNG_INFO_tRNS)
 {
 if (trans_color == 0)
            png_error(pp, "validate: unexpected png_get_tRNS (color) result");

 switch (dp->colour_type)
 {

          case 0:
             dp->transparent.red = dp->transparent.green = dp->transparent.blue =
                trans_color->gray;
            dp->has_tRNS = 1;
             break;
 
          case 2:
             dp->transparent.red = trans_color->red;
             dp->transparent.green = trans_color->green;
             dp->transparent.blue = trans_color->blue;
            dp->has_tRNS = 1;
             break;
 
          case 3:
 /* Not expected because it should result in the array case
             * above.
             */
            png_error(pp, "validate: unexpected png_get_tRNS result");
 break;

 default:
            png_error(pp, "validate: invalid tRNS chunk with alpha image");
 }
 }
 }

 /* Read the number of passes - expected to match the value used when
    * creating the image (interlaced or not).  This has the side effect of

     * turning on interlace handling (if do_interlace is not set.)
     */
    dp->npasses = npasses_from_interlace_type(pp, dp->interlace_type);
   if (!dp->do_interlace)
   {
#     ifdef PNG_READ_INTERLACING_SUPPORTED
         if (dp->npasses != png_set_interlace_handling(pp))
            png_error(pp, "validate: file changed interlace type");
#     else /* !READ_INTERLACING */
         /* This should never happen: the relevant tests (!do_interlace) should
          * not be run.
          */
         if (dp->npasses > 1)
            png_error(pp, "validate: no libpng interlace support");
#     endif /* !READ_INTERLACING */
   }
 
    /* Caller calls png_read_update_info or png_start_read_image now, then calls
     * part2.
    */
}
