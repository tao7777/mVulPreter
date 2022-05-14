 image_transform_png_set_expand_gray_1_2_4_to_8_add(image_transform *this,
    const image_transform **that, png_byte colour_type, png_byte bit_depth)
 {
#if PNG_LIBPNG_VER < 10700
    return image_transform_png_set_expand_add(this, that, colour_type,
       bit_depth);
#else
   UNUSED(bit_depth)

   this->next = *that;
   *that = this;

   /* This should do nothing unless the color type is gray and the bit depth is
    * less than 8:
    */
   return colour_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8;
#endif /* 1.7 or later */
 }
