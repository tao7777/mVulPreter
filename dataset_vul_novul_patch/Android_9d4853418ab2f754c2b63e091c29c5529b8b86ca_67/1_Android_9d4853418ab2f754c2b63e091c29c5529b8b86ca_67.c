image_transform_png_set_expand_mod(PNG_CONST image_transform *this,
     image_pixel *that, png_const_structp pp,
    PNG_CONST transform_display *display)
 {
    /* The general expand case depends on what the colour type is: */
    if (that->colour_type == PNG_COLOR_TYPE_PALETTE)
      image_pixel_convert_PLTE(that);
 else if (that->bit_depth < 8) /* grayscale */

       that->sample_depth = that->bit_depth = 8;
 
    if (that->have_tRNS)
      image_pixel_add_alpha(that, &display->this);
 
    this->next->mod(this->next, that, pp, display);
 }
