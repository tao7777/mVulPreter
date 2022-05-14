image_transform_png_set_tRNS_to_alpha_mod(PNG_CONST image_transform *this,
image_transform_png_set_tRNS_to_alpha_mod(const image_transform *this,
    image_pixel *that, png_const_structp pp,
   const transform_display *display)
 {
#if PNG_LIBPNG_VER < 10700
    /* LIBPNG BUG: this always forces palette images to RGB. */
    if (that->colour_type == PNG_COLOR_TYPE_PALETTE)
       image_pixel_convert_PLTE(that);
#endif
 
    /* This effectively does an 'expand' only if there is some transparency to
     * convert to an alpha channel.
     */
    if (that->have_tRNS)
#     if PNG_LIBPNG_VER >= 10700
         if (that->colour_type != PNG_COLOR_TYPE_PALETTE &&
             (that->colour_type & PNG_COLOR_MASK_ALPHA) == 0)
#     endif
      image_pixel_add_alpha(that, &display->this, 0/*!for background*/);
 
#if PNG_LIBPNG_VER < 10700
    /* LIBPNG BUG: otherwise libpng still expands to 8 bits! */
    else
    {
 if (that->bit_depth < 8)
         that->bit_depth =8;

       if (that->sample_depth < 8)
          that->sample_depth = 8;
    }
#endif
 
    this->next->mod(this->next, that, pp, display);
 }
