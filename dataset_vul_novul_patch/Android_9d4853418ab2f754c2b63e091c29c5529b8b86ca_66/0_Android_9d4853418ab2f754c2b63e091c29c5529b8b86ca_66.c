 image_transform_png_set_expand_gray_1_2_4_to_8_set(
    const image_transform *this, transform_display *that, png_structp pp,
     png_infop pi)
 {
    png_set_expand_gray_1_2_4_to_8(pp);
   /* NOTE: don't expect this to expand tRNS */
    this->next->set(this->next, that, pp, pi);
 }
