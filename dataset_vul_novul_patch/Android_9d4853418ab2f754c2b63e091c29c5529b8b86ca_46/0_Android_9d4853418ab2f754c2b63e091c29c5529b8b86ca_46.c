gamma_image_validate(gamma_display *dp, png_const_structp pp,

    png_infop pi)
 {
    /* Get some constants derived from the input and output file formats: */
   const png_store* const ps = dp->this.ps;
   const png_byte in_ct = dp->this.colour_type;
   const png_byte in_bd = dp->this.bit_depth;
   const png_uint_32 w = dp->this.w;
   const png_uint_32 h = dp->this.h;
   const size_t cbRow = dp->this.cbRow;
   const png_byte out_ct = png_get_color_type(pp, pi);
   const png_byte out_bd = png_get_bit_depth(pp, pi);
 
    /* There are three sources of error, firstly the quantization in the
     * file encoding, determined by sbit and/or the file depth, secondly
    * the output (screen) gamma and thirdly the output file encoding.
    *
    * Since this API receives the screen and file gamma in double
    * precision it is possible to calculate an exact answer given an input
    * pixel value.  Therefore we assume that the *input* value is exact -
    * sample/maxsample - calculate the corresponding gamma corrected
    * output to the limits of double precision arithmetic and compare with
    * what libpng returns.
    *
    * Since the library must quantize the output to 8 or 16 bits there is
    * a fundamental limit on the accuracy of the output of +/-.5 - this
    * quantization limit is included in addition to the other limits
    * specified by the paramaters to the API.  (Effectively, add .5
    * everywhere.)
    *
    * The behavior of the 'sbit' paramter is defined by section 12.5
    * (sample depth scaling) of the PNG spec.  That section forces the
    * decoder to assume that the PNG values have been scaled if sBIT is
    * present:
    *
    *     png-sample = floor( input-sample * (max-out/max-in) + .5);
    *
    * This means that only a subset of the possible PNG values should
    * appear in the input. However, the spec allows the encoder to use a
    * variety of approximations to the above and doesn't require any
    * restriction of the values produced.
    *
    * Nevertheless the spec requires that the upper 'sBIT' bits of the
    * value stored in a PNG file be the original sample bits.
    * Consequently the code below simply scales the top sbit bits by
    * (1<<sbit)-1 to obtain an original sample value.
    *
    * Because there is limited precision in the input it is arguable that
    * an acceptable result is any valid result from input-.5 to input+.5.

     * The basic tests below do not do this, however if 'use_input_precision'
     * is set a subsequent test is performed above.
     */
   const unsigned int samples_per_pixel = (out_ct & 2U) ? 3U : 1U;
    int processing;
    png_uint_32 y;
   const store_palette_entry *in_palette = dp->this.palette;
   const int in_is_transparent = dp->this.is_transparent;
   int process_tRNS;
    int out_npalette = -1;
    int out_is_transparent = 0; /* Just refers to the palette case */
    store_palette out_palette;
   validate_info vi;

 /* Check for row overwrite errors */
   store_image_check(dp->this.ps, pp, 0);

 /* Supply the input and output sample depths here - 8 for an indexed image,
    * otherwise the bit depth.
    */
   init_validate_info(&vi, dp, pp, in_ct==3?8:in_bd, out_ct==3?8:out_bd);

 
    processing = (vi.gamma_correction > 0 && !dp->threshold_test)
       || in_bd != out_bd || in_ct != out_ct || vi.do_background;
   process_tRNS = dp->this.has_tRNS && vi.do_background;
 
    /* TODO: FIX THIS: MAJOR BUG!  If the transformations all happen inside
     * the palette there is no way of finding out, because libpng fails to
    * update the palette on png_read_update_info.  Indeed, libpng doesn't
    * even do the required work until much later, when it doesn't have any
    * info pointer.  Oops.  For the moment 'processing' is turned off if
    * out_ct is palette.
    */
 if (in_ct == 3 && out_ct == 3)
      processing = 0;

 if (processing && out_ct == 3)
      out_is_transparent = read_palette(out_palette, &out_npalette, pp, pi);

 for (y=0; y<h; ++y)
 {
      png_const_bytep pRow = store_image_row(ps, pp, 0, y);
      png_byte std[STANDARD_ROWMAX];

      transform_row(pp, std, in_ct, in_bd, y);

 if (processing)
 {
 unsigned int x;

 for (x=0; x<w; ++x)
 {

             double alpha = 1; /* serves as a flag value */
 
             /* Record the palette index for index images. */
            const unsigned int in_index =
               in_ct == 3 ? sample(std, 3, in_bd, x, 0, 0, 0) : 256;
            const unsigned int out_index =
               out_ct == 3 ? sample(std, 3, out_bd, x, 0, 0, 0) : 256;
 
             /* Handle input alpha - png_set_background will cause the output
              * alpha to disappear so there is nothing to check.
              */
            if ((in_ct & PNG_COLOR_MASK_ALPHA) != 0 ||
                (in_ct == 3 && in_is_transparent))
             {
               const unsigned int input_alpha = in_ct == 3 ?
                   dp->this.palette[in_index].alpha :
                  sample(std, in_ct, in_bd, x, samples_per_pixel, 0, 0);
 
                unsigned int output_alpha = 65536 /* as a flag value */;
 
 if (out_ct == 3)
 {
 if (out_is_transparent)
                     output_alpha = out_palette[out_index].alpha;
 }

 
                else if ((out_ct & PNG_COLOR_MASK_ALPHA) != 0)
                   output_alpha = sample(pRow, out_ct, out_bd, x,
                     samples_per_pixel, 0, 0);
 
                if (output_alpha != 65536)
                   alpha = gamma_component_validate("alpha", &vi, input_alpha,
                     output_alpha, -1/*alpha*/, 0/*background*/);

 else /* no alpha in output */
 {
 /* This is a copy of the calculation of 'i' above in order to
                   * have the alpha value to use in the background calculation.
                   */
                  alpha = input_alpha >> vi.isbit_shift;
                  alpha /= vi.sbit_max;

                }
             }
 
            else if (process_tRNS)
            {
               /* alpha needs to be set appropriately for this pixel, it is
                * currently 1 and needs to be 0 for an input pixel which matches
                * the values in tRNS.
                */
               switch (in_ct)
               {
                  case 0: /* gray */
                     if (sample(std, in_ct, in_bd, x, 0, 0, 0) ==
                           dp->this.transparent.red)
                        alpha = 0;
                     break;

                  case 2: /* RGB */
                     if (sample(std, in_ct, in_bd, x, 0, 0, 0) ==
                           dp->this.transparent.red &&
                         sample(std, in_ct, in_bd, x, 1, 0, 0) ==
                           dp->this.transparent.green &&
                         sample(std, in_ct, in_bd, x, 2, 0, 0) ==
                           dp->this.transparent.blue)
                        alpha = 0;
                     break;

                  default:
                     break;
               }
            }

             /* Handle grayscale or RGB components. */
             if ((in_ct & PNG_COLOR_MASK_COLOR) == 0) /* grayscale */
                (void)gamma_component_validate("gray", &vi,
                  sample(std, in_ct, in_bd, x, 0, 0, 0),
                  sample(pRow, out_ct, out_bd, x, 0, 0, 0),
                  alpha/*component*/, vi.background_red);
             else /* RGB or palette */
             {
                (void)gamma_component_validate("red", &vi,
                   in_ct == 3 ? in_palette[in_index].red :
                     sample(std, in_ct, in_bd, x, 0, 0, 0),
                   out_ct == 3 ? out_palette[out_index].red :
                     sample(pRow, out_ct, out_bd, x, 0, 0, 0),
                   alpha/*component*/, vi.background_red);
 
                (void)gamma_component_validate("green", &vi,
                   in_ct == 3 ? in_palette[in_index].green :
                     sample(std, in_ct, in_bd, x, 1, 0, 0),
                   out_ct == 3 ? out_palette[out_index].green :
                     sample(pRow, out_ct, out_bd, x, 1, 0, 0),
                   alpha/*component*/, vi.background_green);
 
                (void)gamma_component_validate("blue", &vi,
                   in_ct == 3 ? in_palette[in_index].blue :
                     sample(std, in_ct, in_bd, x, 2, 0, 0),
                   out_ct == 3 ? out_palette[out_index].blue :
                     sample(pRow, out_ct, out_bd, x, 2, 0, 0),
                   alpha/*component*/, vi.background_blue);
             }
          }
 }

 else if (memcmp(std, pRow, cbRow) != 0)
 {
 char msg[64];

 /* No transform is expected on the threshold tests. */
         sprintf(msg, "gamma: below threshold row %lu changed",
 (unsigned long)y);

         png_error(pp, msg);
 }
 } /* row (y) loop */

   dp->this.ps->validated = 1;
}
