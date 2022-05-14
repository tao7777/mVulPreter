image_transform_png_set_palette_to_rgb_mod(PNG_CONST image_transform *this,
image_transform_png_set_palette_to_rgb_mod(const image_transform *this,
     image_pixel *that, png_const_structp pp,
    const transform_display *display)
 {
    if (that->colour_type == PNG_COLOR_TYPE_PALETTE)
       image_pixel_convert_PLTE(that);

 this->next->mod(this->next, that, pp, display);
}
