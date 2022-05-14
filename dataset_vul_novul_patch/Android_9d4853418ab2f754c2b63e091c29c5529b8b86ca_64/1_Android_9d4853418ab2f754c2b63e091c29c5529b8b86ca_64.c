 image_transform_png_set_expand_gray_1_2_4_to_8_add(image_transform *this,
    PNG_CONST image_transform **that, png_byte colour_type, png_byte bit_depth)
 {
    return image_transform_png_set_expand_add(this, that, colour_type,
       bit_depth);
 }
