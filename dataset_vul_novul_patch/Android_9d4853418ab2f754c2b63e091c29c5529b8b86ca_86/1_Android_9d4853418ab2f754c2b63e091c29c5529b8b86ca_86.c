image_transform_png_set_strip_alpha_mod(PNG_CONST image_transform *this,
     image_pixel *that, png_const_structp pp,
    PNG_CONST transform_display *display)
 {
    if (that->colour_type == PNG_COLOR_TYPE_GRAY_ALPHA)
       that->colour_type = PNG_COLOR_TYPE_GRAY;
 else if (that->colour_type == PNG_COLOR_TYPE_RGB_ALPHA)
      that->colour_type = PNG_COLOR_TYPE_RGB;

   that->have_tRNS = 0;
   that->alphaf = 1;

 this->next->mod(this->next, that, pp, display);
}
