image_transform_png_set_strip_16_set(PNG_CONST image_transform *this,
image_transform_png_set_strip_16_set(const image_transform *this,
     transform_display *that, png_structp pp, png_infop pi)
 {
    png_set_strip_16(pp);
#  if PNG_LIBPNG_VER < 10700
      /* libpng will limit the gamma table size: */
      that->max_gamma_8 = PNG_MAX_GAMMA_8;
#  endif
    this->next->set(this->next, that, pp, pi);
 }
