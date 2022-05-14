image_transform_png_set_@_mod(PNG_CONST image_transform *this,
    image_pixel *that, png_const_structp pp,
    PNG_CONST transform_display *display)
{
   this->next->mod(this->next, that, pp, display);
}
