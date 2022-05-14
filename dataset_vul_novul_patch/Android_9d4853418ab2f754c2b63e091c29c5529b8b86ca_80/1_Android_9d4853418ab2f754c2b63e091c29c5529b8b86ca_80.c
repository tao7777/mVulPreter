image_transform_png_set_scale_16_mod(PNG_CONST image_transform *this,
     image_pixel *that, png_const_structp pp,
    PNG_CONST transform_display *display)
 {
    if (that->bit_depth == 16)
    {
      that->sample_depth = that->bit_depth = 8;
 if (that->red_sBIT > 8) that->red_sBIT = 8;
 if (that->green_sBIT > 8) that->green_sBIT = 8;
 if (that->blue_sBIT > 8) that->blue_sBIT = 8;
 if (that->alpha_sBIT > 8) that->alpha_sBIT = 8;
 }

 this->next->mod(this->next, that, pp, display);
}
