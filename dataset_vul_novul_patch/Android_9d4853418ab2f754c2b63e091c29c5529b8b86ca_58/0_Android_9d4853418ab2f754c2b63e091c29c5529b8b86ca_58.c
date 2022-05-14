image_transform_png_set_background_mod(PNG_CONST image_transform *this,
image_transform_png_set_background_mod(const image_transform *this,
     image_pixel *that, png_const_structp pp,
    const transform_display *display)
 {
    /* Check for tRNS first: */
    if (that->have_tRNS && that->colour_type != PNG_COLOR_TYPE_PALETTE)
      image_pixel_add_alpha(that, &display->this, 1/*for background*/);
 
    /* This is only necessary if the alpha value is less than 1. */
    if (that->alphaf < 1)
 {
 /* Now we do the background calculation without any gamma correction. */
 if (that->alphaf <= 0)
 {
         that->redf = data.redf;
         that->greenf = data.greenf;
         that->bluef = data.bluef;

         that->rede = data.rede;
         that->greene = data.greene;
         that->bluee = data.bluee;

         that->red_sBIT= data.red_sBIT;
         that->green_sBIT= data.green_sBIT;
         that->blue_sBIT= data.blue_sBIT;
 }

 else /* 0 < alpha < 1 */
 {
 double alf = 1 - that->alphaf;

         that->redf = that->redf * that->alphaf + data.redf * alf;
         that->rede = that->rede * that->alphaf + data.rede * alf +
            DBL_EPSILON;
         that->greenf = that->greenf * that->alphaf + data.greenf * alf;
         that->greene = that->greene * that->alphaf + data.greene * alf +
            DBL_EPSILON;
         that->bluef = that->bluef * that->alphaf + data.bluef * alf;
         that->bluee = that->bluee * that->alphaf + data.bluee * alf +
            DBL_EPSILON;
 }


       /* Remove the alpha type and set the alpha (not in that order.) */
       that->alphaf = 1;
       that->alphae = 0;
    }
 
   if (that->colour_type == PNG_COLOR_TYPE_RGB_ALPHA)
      that->colour_type = PNG_COLOR_TYPE_RGB;
   else if (that->colour_type == PNG_COLOR_TYPE_GRAY_ALPHA)
      that->colour_type = PNG_COLOR_TYPE_GRAY;
   /* PNG_COLOR_TYPE_PALETTE is not changed */

    this->next->mod(this->next, that, pp, display);
 }
