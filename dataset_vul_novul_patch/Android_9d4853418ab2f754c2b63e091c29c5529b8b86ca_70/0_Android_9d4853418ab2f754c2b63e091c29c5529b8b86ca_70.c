image_transform_png_set_gray_to_rgb_mod(PNG_CONST image_transform *this,
image_transform_png_set_gray_to_rgb_mod(const image_transform *this,
     image_pixel *that, png_const_structp pp,
    const transform_display *display)
 {
    /* NOTE: we can actually pend the tRNS processing at this point because we
     * can correctly recognize the original pixel value even though we have
    * mapped the one gray channel to the three RGB ones, but in fact libpng

     * doesn't do this, so we don't either.
     */
    if ((that->colour_type & PNG_COLOR_MASK_COLOR) == 0 && that->have_tRNS)
      image_pixel_add_alpha(that, &display->this, 0/*!for background*/);
 
    /* Simply expand the bit depth and alter the colour type as required. */
    if (that->colour_type == PNG_COLOR_TYPE_GRAY)
 {
 /* RGB images have a bit depth at least equal to '8' */
 if (that->bit_depth < 8)
         that->sample_depth = that->bit_depth = 8;

 /* And just changing the colour type works here because the green and blue
       * channels are being maintained in lock-step with the red/gray:
       */
      that->colour_type = PNG_COLOR_TYPE_RGB;
 }

 else if (that->colour_type == PNG_COLOR_TYPE_GRAY_ALPHA)
      that->colour_type = PNG_COLOR_TYPE_RGB_ALPHA;

 this->next->mod(this->next, that, pp, display);
}
