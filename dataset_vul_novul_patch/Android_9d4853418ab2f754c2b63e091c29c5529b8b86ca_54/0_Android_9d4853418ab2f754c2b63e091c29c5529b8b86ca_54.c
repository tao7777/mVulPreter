 image_transform_default_add(image_transform *this,
    const image_transform **that, png_byte colour_type, png_byte bit_depth)
 {
    UNUSED(colour_type)
    UNUSED(bit_depth)

 this->next = *that;
 *that = this;

 return 1;
}
