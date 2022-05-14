image_pixel_add_alpha(image_pixel *this, PNG_CONST standard_display *display)
 {
    if (this->colour_type == PNG_COLOR_TYPE_PALETTE)
       image_pixel_convert_PLTE(this);

 if ((this->colour_type & PNG_COLOR_MASK_ALPHA) == 0)

    {
       if (this->colour_type == PNG_COLOR_TYPE_GRAY)
       {
         if (this->bit_depth < 8)
            this->bit_depth = 8;
 
          if (this->have_tRNS)
          {
             this->have_tRNS = 0;
 
             /* Check the input, original, channel value here against the
             * original tRNS gray chunk valie.
             */
 if (this->red == display->transparent.red)
 this->alphaf = 0;
 else
 this->alphaf = 1;
 }
 else
 this->alphaf = 1;

 this->colour_type = PNG_COLOR_TYPE_GRAY_ALPHA;
 }

 else if (this->colour_type == PNG_COLOR_TYPE_RGB)
 {
 if (this->have_tRNS)
 {
 this->have_tRNS = 0;

 /* Again, check the exact input values, not the current transformed
             * value!
             */
 if (this->red == display->transparent.red &&
 this->green == display->transparent.green &&
 this->blue == display->transparent.blue)

                this->alphaf = 0;
             else
                this->alphaf = 1;
            this->colour_type = PNG_COLOR_TYPE_RGB_ALPHA;
          }
       }
 
       /* The error in the alpha is zero and the sBIT value comes from the
       * original sBIT data (actually it will always be the original bit depth).
       */
 this->alphae = 0;

       this->alpha_sBIT = display->alpha_sBIT;
    }
 }
