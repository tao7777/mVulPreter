image_transform_png_set_expand_16_set(PNG_CONST image_transform *this,
image_transform_png_set_expand_16_set(const image_transform *this,
     transform_display *that, png_structp pp, png_infop pi)
 {
    png_set_expand_16(pp);

   /* NOTE: prior to 1.7 libpng does SET_EXPAND as well, so tRNS is expanded. */
#  if PNG_LIBPNG_VER < 10700
      if (that->this.has_tRNS)
         that->this.is_transparent = 1;
#  endif

    this->next->set(this->next, that, pp, pi);
 }
