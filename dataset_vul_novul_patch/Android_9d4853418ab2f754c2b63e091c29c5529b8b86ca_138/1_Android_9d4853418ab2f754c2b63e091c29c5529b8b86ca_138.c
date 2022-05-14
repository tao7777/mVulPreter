store_image_check(PNG_CONST png_store* ps, png_const_structp pp, int iImage)
 {
    png_const_bytep image = ps->image;
 
 if (image[-1] != 0xed || image[ps->cb_image] != 0xfe)
      png_error(pp, "image overwrite");
 else
 {
 png_size_t cbRow = ps->cb_row;
      png_uint_32 rows = ps->image_h;

      image += iImage * (cbRow+5) * ps->image_h;

      image += 2; /* skip image first row markers */

 while (rows-- > 0)
 {
 if (image[-2] != 190 || image[-1] != 239)
            png_error(pp, "row start overwritten");

 if (image[cbRow] != 222 || image[cbRow+1] != 173 ||
            image[cbRow+2] != 17)
            png_error(pp, "row end overwritten");

         image += cbRow+5;
 }
 }
}
