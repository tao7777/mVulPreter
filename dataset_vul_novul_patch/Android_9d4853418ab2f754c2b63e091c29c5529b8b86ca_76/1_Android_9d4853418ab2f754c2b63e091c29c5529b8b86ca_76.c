image_transform_png_set_rgb_to_gray_ini(PNG_CONST image_transform *this,
     transform_display *that)
 {
    png_modifier *pm = that->pm;
   PNG_CONST color_encoding *e = pm->current_encoding;
 
    UNUSED(this)
 
 /* Since we check the encoding this flag must be set: */
   pm->test_uses_encoding = 1;

 /* If 'e' is not NULL chromaticity information is present and either a cHRM
    * or an sRGB chunk will be inserted.
    */
 if (e != 0)
 {

       /* Coefficients come from the encoding, but may need to be normalized to a
        * white point Y of 1.0
        */
      PNG_CONST double whiteY = e->red.Y + e->green.Y + e->blue.Y;
 
       data.red_coefficient = e->red.Y;
       data.green_coefficient = e->green.Y;
      data.blue_coefficient = e->blue.Y;

 if (whiteY != 1)
 {
         data.red_coefficient /= whiteY;
         data.green_coefficient /= whiteY;
         data.blue_coefficient /= whiteY;
 }
 }


    else
    {
       /* The default (built in) coeffcients, as above: */
      data.red_coefficient = 6968 / 32768.;
      data.green_coefficient = 23434 / 32768.;
      data.blue_coefficient = 2366 / 32768.;
    }
 
    data.gamma = pm->current_gamma;

 /* If not set then the calculations assume linear encoding (implicitly): */
 if (data.gamma == 0)
      data.gamma = 1;

 /* The arguments to png_set_rgb_to_gray can override the coefficients implied
    * by the color space encoding.  If doing exhaustive checks do the override
    * in each case, otherwise do it randomly.
    */
 if (pm->test_exhaustive)
 {
 /* First time in coefficients_overridden is 0, the following sets it to 1,
       * so repeat if it is set.  If a test fails this may mean we subsequently
       * skip a non-override test, ignore that.
       */
      data.coefficients_overridden = !data.coefficients_overridden;
      pm->repeat = data.coefficients_overridden != 0;
 }

 else
      data.coefficients_overridden = random_choice();

 if (data.coefficients_overridden)
 {
 /* These values override the color encoding defaults, simply use random
       * numbers.
       */
      png_uint_32 ru;
 double total;

      RANDOMIZE(ru);
      data.green_coefficient = total = (ru & 0xffff) / 65535.;
      ru >>= 16;
      data.red_coefficient = (1 - total) * (ru & 0xffff) / 65535.;
      total += data.red_coefficient;
      data.blue_coefficient = 1 - total;

#     ifdef PNG_FLOATING_POINT_SUPPORTED
         data.red_to_set = data.red_coefficient;
         data.green_to_set = data.green_coefficient;
#     else
         data.red_to_set = fix(data.red_coefficient);
         data.green_to_set = fix(data.green_coefficient);
#     endif

 /* The following just changes the error messages: */
      pm->encoding_ignored = 1;
 }

 else
 {
      data.red_to_set = -1;
      data.green_to_set = -1;
 }

 /* Adjust the error limit in the png_modifier because of the larger errors
    * produced in the digitization during the gamma handling.
    */
 if (data.gamma != 1) /* Use gamma tables */
 {
 if (that->this.bit_depth == 16 || pm->assume_16_bit_calculations)
 {
 /* The computations have the form:
          *
          *    r * rc + g * gc + b * bc
          *
          *  Each component of which is +/-1/65535 from the gamma_to_1 table
          *  lookup, resulting in a base error of +/-6.  The gamma_from_1

           *  conversion adds another +/-2 in the 16-bit case and
           *  +/-(1<<(15-PNG_MAX_GAMMA_8)) in the 8-bit case.
           */
          that->pm->limit += pow(
#           if PNG_MAX_GAMMA_8 < 14
               (that->this.bit_depth == 16 ? 8. :
                  6. + (1<<(15-PNG_MAX_GAMMA_8)))
#           else
               8.
#           endif
               /65535, data.gamma);
       }
 
       else
 {
 /* Rounding to 8 bits in the linear space causes massive errors which
          * will trigger the error check in transform_range_check.  Fix that
          * here by taking the gamma encoding into account.
          *

           * When DIGITIZE is set because a pre-1.7 version of libpng is being
           * tested allow a bigger slack.
           *
          * NOTE: this magic number was determined by experiment to be 1.1 (when
          * using fixed point arithmetic).  There's no great merit to the value
          * below, however it only affects the limit used for checking for
          * internal calculation errors, not the actual limit imposed by
          * pngvalid on the output errors.
           */
          that->pm->limit += pow(
#           if DIGITIZE
               1.1
#           else
               1.
#           endif
               /255, data.gamma);
       }
    }
 
 else
 {
 /* With no gamma correction a large error comes from the truncation of the
       * calculation in the 8 bit case, allow for that here.
       */
 if (that->this.bit_depth != 16 && !pm->assume_16_bit_calculations)
         that->pm->limit += 4E-3;
 }

 }
