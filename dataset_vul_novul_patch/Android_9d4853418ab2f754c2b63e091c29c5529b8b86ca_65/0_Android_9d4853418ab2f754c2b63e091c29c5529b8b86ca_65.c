 image_transform_png_set_expand_gray_1_2_4_to_8_mod(
    const image_transform *this, image_pixel *that, png_const_structp pp,
    const transform_display *display)
 {
#if PNG_LIBPNG_VER < 10700
    image_transform_png_set_expand_mod(this, that, pp, display);
#else
   /* Only expand grayscale of bit depth less than 8: */
   if (that->colour_type == PNG_COLOR_TYPE_GRAY &&
       that->bit_depth < 8)
      that->sample_depth = that->bit_depth = 8;

   this->next->mod(this->next, that, pp, display);
#endif /* 1.7 or later */
 }
