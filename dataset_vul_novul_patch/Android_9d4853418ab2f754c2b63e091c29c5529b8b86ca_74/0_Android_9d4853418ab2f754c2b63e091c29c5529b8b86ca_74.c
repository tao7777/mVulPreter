image_transform_png_set_palette_to_rgb_set(PNG_CONST image_transform *this,
image_transform_png_set_palette_to_rgb_set(const image_transform *this,
     transform_display *that, png_structp pp, png_infop pi)
 {
    png_set_palette_to_rgb(pp);
 this->next->set(this->next, that, pp, pi);

 }
