 image_transform_png_set_rgb_to_gray_add(image_transform *this,
    const image_transform **that, png_byte colour_type, png_byte bit_depth)
 {
    UNUSED(bit_depth)
 
 this->next = *that;
 *that = this;

 return (colour_type & PNG_COLOR_MASK_COLOR) != 0;
}
