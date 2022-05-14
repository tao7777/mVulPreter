image_transform_mod_end(PNG_CONST image_transform *this, image_pixel *that,
    png_const_structp pp, PNG_CONST transform_display *display)
 {
   PNG_CONST unsigned int scale = (1U<<that->sample_depth)-1;
 
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
       that->blue = sample_scale(that->bluef, scale);
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
      that->alpha = 1;     /* Override this. */
       that->alphae = 0;    /* It's exact ;-) */
    }
 }
