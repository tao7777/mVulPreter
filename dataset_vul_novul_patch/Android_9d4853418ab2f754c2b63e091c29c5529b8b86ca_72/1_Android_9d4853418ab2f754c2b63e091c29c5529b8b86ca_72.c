 image_transform_png_set_palette_to_rgb_add(image_transform *this,
    PNG_CONST image_transform **that, png_byte colour_type, png_byte bit_depth)
 {
    UNUSED(bit_depth)
 
 this->next = *that;
 *that = this;

 return colour_type == PNG_COLOR_TYPE_PALETTE;
}
