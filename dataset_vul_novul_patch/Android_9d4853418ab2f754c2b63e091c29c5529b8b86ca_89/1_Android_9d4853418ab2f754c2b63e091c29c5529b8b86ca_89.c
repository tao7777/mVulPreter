image_transform_png_set_tRNS_to_alpha_mod(PNG_CONST image_transform *this,
    image_pixel *that, png_const_structp pp,
   PNG_CONST transform_display *display)
 {
    /* LIBPNG BUG: this always forces palette images to RGB. */
    if (that->colour_type == PNG_COLOR_TYPE_PALETTE)
       image_pixel_convert_PLTE(that);
 
    /* This effectively does an 'expand' only if there is some transparency to
     * convert to an alpha channel.
     */
    if (that->have_tRNS)
      image_pixel_add_alpha(that, &display->this);
 
    /* LIBPNG BUG: otherwise libpng still expands to 8 bits! */
    else
    {
 if (that->bit_depth < 8)
         that->bit_depth =8;

       if (that->sample_depth < 8)
          that->sample_depth = 8;
    }
 
    this->next->mod(this->next, that, pp, display);
 }
