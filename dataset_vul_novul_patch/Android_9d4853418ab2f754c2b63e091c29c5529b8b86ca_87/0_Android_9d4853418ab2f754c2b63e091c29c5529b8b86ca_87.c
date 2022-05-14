image_transform_png_set_strip_alpha_set(PNG_CONST image_transform *this,
image_transform_png_set_strip_alpha_set(const image_transform *this,
     transform_display *that, png_structp pp, png_infop pi)
 {
    png_set_strip_alpha(pp);
 this->next->set(this->next, that, pp, pi);

 }
