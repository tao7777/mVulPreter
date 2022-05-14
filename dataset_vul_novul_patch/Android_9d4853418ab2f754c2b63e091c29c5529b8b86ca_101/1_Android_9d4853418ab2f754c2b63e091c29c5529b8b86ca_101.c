modifier_color_encoding_is_sRGB(PNG_CONST png_modifier *pm)
 {
    return pm->current_encoding != 0 && pm->current_encoding == pm->encodings &&
       pm->current_encoding->gamma == pm->current_gamma;
 }
