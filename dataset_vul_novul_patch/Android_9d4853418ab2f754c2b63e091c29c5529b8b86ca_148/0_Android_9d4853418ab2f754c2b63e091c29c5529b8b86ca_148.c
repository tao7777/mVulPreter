transform_image_validate(transform_display *dp, png_const_structp pp,

    png_infop pi)
 {
    /* Constants for the loop below: */
   const png_store* const ps = dp->this.ps;
   const png_byte in_ct = dp->this.colour_type;
   const png_byte in_bd = dp->this.bit_depth;
   const png_uint_32 w = dp->this.w;
   const png_uint_32 h = dp->this.h;
   const png_byte out_ct = dp->output_colour_type;
   const png_byte out_bd = dp->output_bit_depth;
   const png_byte sample_depth = (png_byte)(out_ct ==
       PNG_COLOR_TYPE_PALETTE ? 8 : out_bd);
   const png_byte red_sBIT = dp->this.red_sBIT;
   const png_byte green_sBIT = dp->this.green_sBIT;
   const png_byte blue_sBIT = dp->this.blue_sBIT;
   const png_byte alpha_sBIT = dp->this.alpha_sBIT;
   const int have_tRNS = dp->this.is_transparent;
    double digitization_error;
 
    store_palette out_palette;
   png_uint_32 y;

   UNUSED(pi)

 /* Check for row overwrite errors */
   store_image_check(dp->this.ps, pp, 0);

 /* Read the palette corresponding to the output if the output colour type
    * indicates a palette, othewise set out_palette to garbage.
    */
 if (out_ct == PNG_COLOR_TYPE_PALETTE)
 {
 /* Validate that the palette count itself has not changed - this is not
       * expected.
       */
 int npalette = (-1);

 (void)read_palette(out_palette, &npalette, pp, pi);
 if (npalette != dp->this.npalette)
         png_error(pp, "unexpected change in palette size");

      digitization_error = .5;
 }
 else
 {
      png_byte in_sample_depth;

      memset(out_palette, 0x5e, sizeof out_palette);

 /* use-input-precision means assume that if the input has 8 bit (or less)
       * samples and the output has 16 bit samples the calculations will be done
       * with 8 bit precision, not 16.
       */
 if (in_ct == PNG_COLOR_TYPE_PALETTE || in_bd < 16)
         in_sample_depth = 8;
 else
         in_sample_depth = in_bd;

 if (sample_depth != 16 || in_sample_depth > 8 ||
 !dp->pm->calculations_use_input_precision)
         digitization_error = .5;

 /* Else calculations are at 8 bit precision, and the output actually
       * consists of scaled 8-bit values, so scale .5 in 8 bits to the 16 bits:
       */
 else
         digitization_error = .5 * 257;
 }

 
    for (y=0; y<h; ++y)
    {
      png_const_bytep const pRow = store_image_row(ps, pp, 0, y);
       png_uint_32 x;
 
       /* The original, standard, row pre-transforms. */
      png_byte std[STANDARD_ROWMAX];

      transform_row(pp, std, in_ct, in_bd, y);

 /* Go through each original pixel transforming it and comparing with what
       * libpng did to the same pixel.
       */
 for (x=0; x<w; ++x)
 {
         image_pixel in_pixel, out_pixel;

          unsigned int r, g, b, a;
 
          /* Find out what we think the pixel should be: */
         image_pixel_init(&in_pixel, std, in_ct, in_bd, x, dp->this.palette,
                 NULL);
 
          in_pixel.red_sBIT = red_sBIT;
          in_pixel.green_sBIT = green_sBIT;
          in_pixel.blue_sBIT = blue_sBIT;
          in_pixel.alpha_sBIT = alpha_sBIT;
         in_pixel.have_tRNS = have_tRNS != 0;
 
          /* For error detection, below. */
          r = in_pixel.red;
         g = in_pixel.green;

          b = in_pixel.blue;
          a = in_pixel.alpha;
 
         /* This applies the transforms to the input data, including output
          * format operations which must be used when reading the output
          * pixel that libpng produces.
          */
          dp->transform_list->mod(dp->transform_list, &in_pixel, pp, dp);
 
          /* Read the output pixel and compare it to what we got, we don't
          * use the error field here, so no need to update sBIT.  in_pixel
          * says whether we expect libpng to change the output format.
           */
         image_pixel_init(&out_pixel, pRow, out_ct, out_bd, x, out_palette,
                 &in_pixel);
 
          /* We don't expect changes to the index here even if the bit depth is
           * changed.
          */
 if (in_ct == PNG_COLOR_TYPE_PALETTE &&
            out_ct == PNG_COLOR_TYPE_PALETTE)
 {
 if (in_pixel.palette_index != out_pixel.palette_index)
               png_error(pp, "unexpected transformed palette index");
 }

 /* Check the colours for palette images too - in fact the palette could
          * be separately verified itself in most cases.
          */
 if (in_pixel.red != out_pixel.red)
            transform_range_check(pp, r, g, b, a, in_pixel.red, in_pixel.redf,
               out_pixel.red, sample_depth, in_pixel.rede,
               dp->pm->limit + 1./(2*((1U<<in_pixel.red_sBIT)-1)), "red/gray",
               digitization_error);

 if ((out_ct & PNG_COLOR_MASK_COLOR) != 0 &&
            in_pixel.green != out_pixel.green)
            transform_range_check(pp, r, g, b, a, in_pixel.green,
               in_pixel.greenf, out_pixel.green, sample_depth, in_pixel.greene,
               dp->pm->limit + 1./(2*((1U<<in_pixel.green_sBIT)-1)), "green",
               digitization_error);

 if ((out_ct & PNG_COLOR_MASK_COLOR) != 0 &&
            in_pixel.blue != out_pixel.blue)
            transform_range_check(pp, r, g, b, a, in_pixel.blue, in_pixel.bluef,
               out_pixel.blue, sample_depth, in_pixel.bluee,
               dp->pm->limit + 1./(2*((1U<<in_pixel.blue_sBIT)-1)), "blue",
               digitization_error);

 if ((out_ct & PNG_COLOR_MASK_ALPHA) != 0 &&
            in_pixel.alpha != out_pixel.alpha)
            transform_range_check(pp, r, g, b, a, in_pixel.alpha,
               in_pixel.alphaf, out_pixel.alpha, sample_depth, in_pixel.alphae,
               dp->pm->limit + 1./(2*((1U<<in_pixel.alpha_sBIT)-1)), "alpha",
               digitization_error);
 } /* pixel (x) loop */
 } /* row (y) loop */

 /* Record that something was actually checked to avoid a false positive. */
   dp->this.ps->validated = 1;
}
