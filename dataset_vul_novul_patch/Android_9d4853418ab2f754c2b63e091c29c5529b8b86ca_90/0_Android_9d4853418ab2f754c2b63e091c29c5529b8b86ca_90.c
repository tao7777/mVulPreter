image_transform_png_set_tRNS_to_alpha_set(PNG_CONST image_transform *this,
image_transform_png_set_tRNS_to_alpha_set(const image_transform *this,
    transform_display *that, png_structp pp, png_infop pi)
 {
    png_set_tRNS_to_alpha(pp);

   /* If there was a tRNS chunk that would get expanded and add an alpha
    * channel is_transparent must be updated:
    */
   if (that->this.has_tRNS)
      that->this.is_transparent = 1;

    this->next->set(this->next, that, pp, pi);
 }
