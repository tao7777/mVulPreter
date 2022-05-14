image_transform_png_set_strip_16_set(PNG_CONST image_transform *this,
     transform_display *that, png_structp pp, png_infop pi)
 {
    png_set_strip_16(pp);
    this->next->set(this->next, that, pp, pi);
 }
