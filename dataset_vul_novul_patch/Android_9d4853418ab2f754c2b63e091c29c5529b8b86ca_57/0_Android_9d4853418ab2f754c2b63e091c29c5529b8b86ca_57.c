image_transform_mod_end(PNG_CONST image_transform *this, image_pixel *that,
image_transform_mod_end(const image_transform *this, image_pixel *that,
    png_const_structp pp, const transform_display *display)
 {
   const unsigned int scale = (1U<<that->sample_depth)-1;
   const int sig_bits = that->sig_bits;
 
    UNUSED(this)
    UNUSED(pp)
   UNUSED(display)

 /* At the end recalculate the digitized red green and blue values according
    * to the current sample_depth of the pixel.
    *
    * The sample value is simply scaled to the maximum, checking for over
    * and underflow (which can both happen for some image transforms,
    * including simple size scaling, though libpng doesn't do that at present.

     */
    that->red = sample_scale(that->redf, scale);
 
   /* This is a bit bogus; really the above calculation should use the red_sBIT
    * value, not sample_depth, but because libpng does png_set_shift by just
    * shifting the bits we get errors if we don't do it the same way.
    */
   if (sig_bits && that->red_sBIT < that->sample_depth)
      that->red >>= that->sample_depth - that->red_sBIT;

    /* The error value is increased, at the end, according to the lowest sBIT
     * value seen.  Common sense tells us that the intermediate integer
     * representations are no more accurate than +/- 0.5 in the integral values,
    * the sBIT allows the implementation to be worse than this.  In addition the
    * PNG specification actually permits any error within the range (-1..+1),
    * but that is ignored here.  Instead the final digitized value is compared,
    * below to the digitized value of the error limits - this has the net effect
    * of allowing (almost) +/-1 in the output value.  It's difficult to see how
    * any algorithm that digitizes intermediate results can be more accurate.
    */
   that->rede += 1./(2*((1U<<that->red_sBIT)-1));


    if (that->colour_type & PNG_COLOR_MASK_COLOR)
    {
       that->green = sample_scale(that->greenf, scale);
      if (sig_bits && that->green_sBIT < that->sample_depth)
         that->green >>= that->sample_depth - that->green_sBIT;

       that->blue = sample_scale(that->bluef, scale);
      if (sig_bits && that->blue_sBIT < that->sample_depth)
         that->blue >>= that->sample_depth - that->blue_sBIT;

       that->greene += 1./(2*((1U<<that->green_sBIT)-1));
       that->bluee += 1./(2*((1U<<that->blue_sBIT)-1));
    }
 else
 {
      that->blue = that->green = that->red;
      that->bluef = that->greenf = that->redf;
      that->bluee = that->greene = that->rede;
 }

 if ((that->colour_type & PNG_COLOR_MASK_ALPHA) ||
      that->colour_type == PNG_COLOR_TYPE_PALETTE)
 {
      that->alpha = sample_scale(that->alphaf, scale);
      that->alphae += 1./(2*((1U<<that->alpha_sBIT)-1));
 }

    else
    {
       that->alpha = scale; /* opaque */
      that->alphaf = 1;    /* Override this. */
       that->alphae = 0;    /* It's exact ;-) */
    }

   if (sig_bits && that->alpha_sBIT < that->sample_depth)
      that->alpha >>= that->sample_depth - that->alpha_sBIT;
 }
