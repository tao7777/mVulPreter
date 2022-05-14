 image_transform_png_set_tRNS_to_alpha_add(image_transform *this,
    const image_transform **that, png_byte colour_type, png_byte bit_depth)
 {
    UNUSED(bit_depth)
 
 this->next = *that;
 *that = this;

 
    /* We don't know yet whether there will be a tRNS chunk, but we know that
     * this transformation should do nothing if there already is an alpha
    * channel.  In addition, after the bug fix in 1.7.0, there is no longer
    * any action on a palette image.
     */
   return
#  if PNG_LIBPNG_VER >= 10700
      colour_type != PNG_COLOR_TYPE_PALETTE &&
#  endif
   (colour_type & PNG_COLOR_MASK_ALPHA) == 0;
 }
