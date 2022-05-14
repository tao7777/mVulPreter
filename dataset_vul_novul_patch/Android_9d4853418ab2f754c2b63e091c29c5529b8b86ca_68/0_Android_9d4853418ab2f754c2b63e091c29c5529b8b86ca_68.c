image_transform_png_set_expand_set(PNG_CONST image_transform *this,
image_transform_png_set_expand_set(const image_transform *this,
     transform_display *that, png_structp pp, png_infop pi)
 {
    png_set_expand(pp);

   if (that->this.has_tRNS)
      that->this.is_transparent = 1;

    this->next->set(this->next, that, pp, pi);
 }
