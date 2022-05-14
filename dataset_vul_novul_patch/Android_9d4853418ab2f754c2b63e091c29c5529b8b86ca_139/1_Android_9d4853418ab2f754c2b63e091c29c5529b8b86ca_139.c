store_image_row(PNG_CONST png_store* ps, png_const_structp pp, int nImage,
    png_uint_32 y)
 {
    png_size_t coffset = (nImage * ps->image_h + y) * (ps->cb_row + 5) + 2;

 if (ps->image == NULL)
      png_error(pp, "no allocated image");

 if (coffset + ps->cb_row + 3 > ps->cb_image)
      png_error(pp, "image too small");

 return ps->image + coffset;
}
