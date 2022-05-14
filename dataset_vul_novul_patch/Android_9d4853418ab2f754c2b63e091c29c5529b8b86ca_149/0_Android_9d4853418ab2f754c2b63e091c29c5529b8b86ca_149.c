transform_info_imp(transform_display *dp, png_structp pp, png_infop pi)
{
 /* Reuse the standard stuff as appropriate. */
   standard_info_part1(&dp->this, pp, pi);

 /* Now set the list of transforms. */
   dp->transform_list->set(dp->transform_list, dp, pp, pi);

 /* Update the info structure for these transforms: */
 {
 int i = dp->this.use_update_info;
 /* Always do one call, even if use_update_info is 0. */
 do
         png_read_update_info(pp, pi);
 while (--i > 0);
 }

 /* And get the output information into the standard_display */
   standard_info_part2(&dp->this, pp, pi, 1/*images*/);

 /* Plus the extra stuff we need for the transform tests: */

    dp->output_colour_type = png_get_color_type(pp, pi);
    dp->output_bit_depth = png_get_bit_depth(pp, pi);
 
   /* If png_set_filler is in action then fake the output color type to include
    * an alpha channel where appropriate.
    */
   if (dp->output_bit_depth >= 8 &&
       (dp->output_colour_type == PNG_COLOR_TYPE_RGB ||
        dp->output_colour_type == PNG_COLOR_TYPE_GRAY) && dp->this.filler)
       dp->output_colour_type |= 4;

    /* Validate the combination of colour type and bit depth that we are getting
     * out of libpng; the semantics of something not in the PNG spec are, at
     * best, unclear.
    */
 switch (dp->output_colour_type)
 {
 case PNG_COLOR_TYPE_PALETTE:
 if (dp->output_bit_depth > 8) goto error;
 /*FALL THROUGH*/
 case PNG_COLOR_TYPE_GRAY:
 if (dp->output_bit_depth == 1 || dp->output_bit_depth == 2 ||
         dp->output_bit_depth == 4)
 break;
 /*FALL THROUGH*/
 default:
 if (dp->output_bit_depth == 8 || dp->output_bit_depth == 16)
 break;
 /*FALL THROUGH*/
   error:
 {
 char message[128];
 size_t pos;

         pos = safecat(message, sizeof message, 0,
 "invalid final bit depth: colour type(");
         pos = safecatn(message, sizeof message, pos, dp->output_colour_type);
         pos = safecat(message, sizeof message, pos, ") with bit depth: ");
         pos = safecatn(message, sizeof message, pos, dp->output_bit_depth);

         png_error(pp, message);
 }

    }
 
    /* Use a test pixel to check that the output agrees with what we expect -
    * this avoids running the whole test if the output is unexpected.  This also
    * checks for internal errors.
     */
    {
       image_pixel test_pixel;

      memset(&test_pixel, 0, sizeof test_pixel);
      test_pixel.colour_type = dp->this.colour_type; /* input */
      test_pixel.bit_depth = dp->this.bit_depth;
 if (test_pixel.colour_type == PNG_COLOR_TYPE_PALETTE)
         test_pixel.sample_depth = 8;
 else
         test_pixel.sample_depth = test_pixel.bit_depth;

       /* Don't need sBIT here, but it must be set to non-zero to avoid
        * arithmetic overflows.
        */
      test_pixel.have_tRNS = dp->this.is_transparent != 0;
       test_pixel.red_sBIT = test_pixel.green_sBIT = test_pixel.blue_sBIT =
          test_pixel.alpha_sBIT = test_pixel.sample_depth;
 
      dp->transform_list->mod(dp->transform_list, &test_pixel, pp, dp);

 if (test_pixel.colour_type != dp->output_colour_type)
 {
 char message[128];
 size_t pos = safecat(message, sizeof message, 0, "colour type ");

         pos = safecatn(message, sizeof message, pos, dp->output_colour_type);
         pos = safecat(message, sizeof message, pos, " expected ");
         pos = safecatn(message, sizeof message, pos, test_pixel.colour_type);

         png_error(pp, message);
 }

 if (test_pixel.bit_depth != dp->output_bit_depth)
 {
 char message[128];
 size_t pos = safecat(message, sizeof message, 0, "bit depth ");

         pos = safecatn(message, sizeof message, pos, dp->output_bit_depth);
         pos = safecat(message, sizeof message, pos, " expected ");
         pos = safecatn(message, sizeof message, pos, test_pixel.bit_depth);

         png_error(pp, message);

       }
 
       /* If both bit depth and colour type are correct check the sample depth.
        */
      if (test_pixel.colour_type == PNG_COLOR_TYPE_PALETTE &&
          test_pixel.sample_depth != 8) /* oops - internal error! */
         png_error(pp, "pngvalid: internal: palette sample depth not 8");
      else if (dp->unpacked && test_pixel.bit_depth != 8)
         png_error(pp, "pngvalid: internal: bad unpacked pixel depth");
      else if (!dp->unpacked && test_pixel.colour_type != PNG_COLOR_TYPE_PALETTE
              && test_pixel.bit_depth != test_pixel.sample_depth)
       {
          char message[128];
          size_t pos = safecat(message, sizeof message, 0,
             "internal: sample depth ");
 
         /* Because unless something has set 'unpacked' or the image is palette
          * mapped we expect the transform to keep sample depth and bit depth
          * the same.
          */
         pos = safecatn(message, sizeof message, pos, test_pixel.sample_depth);
         pos = safecat(message, sizeof message, pos, " expected ");
         pos = safecatn(message, sizeof message, pos, test_pixel.bit_depth);

         png_error(pp, message);
      }
      else if (test_pixel.bit_depth != dp->output_bit_depth)
      {
         /* This could be a libpng error too; libpng has not produced what we
          * expect for the output bit depth.
          */
         char message[128];
         size_t pos = safecat(message, sizeof message, 0,
            "internal: bit depth ");

          pos = safecatn(message, sizeof message, pos, dp->output_bit_depth);
          pos = safecat(message, sizeof message, pos, " expected ");
         pos = safecatn(message, sizeof message, pos, test_pixel.bit_depth);
 
          png_error(pp, message);
       }
 }
}
