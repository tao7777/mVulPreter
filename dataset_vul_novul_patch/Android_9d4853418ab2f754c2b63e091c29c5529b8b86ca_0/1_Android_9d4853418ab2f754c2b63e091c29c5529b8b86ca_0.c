print_pixel(png_structp png_ptr, png_infop info_ptr, png_const_bytep row,
   png_uint_32 x)
{
   PNG_CONST unsigned int bit_depth = png_get_bit_depth(png_ptr, info_ptr);

 switch (png_get_color_type(png_ptr, info_ptr))
 {
 case PNG_COLOR_TYPE_GRAY:
         printf("GRAY %u\n", component(row, x, 0, bit_depth, 1));
 return;

 /* The palette case is slightly more difficult - the palette and, if
       * present, the tRNS ('transparency', though the values are really
       * opacity) data must be read to give the full picture:

        */
       case PNG_COLOR_TYPE_PALETTE:
          {
            PNG_CONST unsigned int index = component(row, x, 0, bit_depth, 1);
             png_colorp palette = NULL;
             int num_palette = 0;
 
 if ((png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette) &
               PNG_INFO_PLTE) && num_palette > 0 && palette != NULL)
 {
               png_bytep trans_alpha = NULL;
 int num_trans = 0;
 if ((png_get_tRNS(png_ptr, info_ptr, &trans_alpha, &num_trans,
                  NULL) & PNG_INFO_tRNS) && num_trans > 0 &&
                  trans_alpha != NULL)
                  printf("INDEXED %u = %d %d %d %d\n", index,
                     palette[index].red, palette[index].green,
                     palette[index].blue,
                     index < num_trans ? trans_alpha[index] : 255);

 else /* no transparency */
                  printf("INDEXED %u = %d %d %d\n", index,
                     palette[index].red, palette[index].green,
                     palette[index].blue);
 }

 else
               printf("INDEXED %u = invalid index\n", index);
 }
 return;

 case PNG_COLOR_TYPE_RGB:
         printf("RGB %u %u %u\n", component(row, x, 0, bit_depth, 3),
            component(row, x, 1, bit_depth, 3),
            component(row, x, 2, bit_depth, 3));
 return;

 case PNG_COLOR_TYPE_GRAY_ALPHA:
         printf("GRAY+ALPHA %u %u\n", component(row, x, 0, bit_depth, 2),
            component(row, x, 1, bit_depth, 2));
 return;

 case PNG_COLOR_TYPE_RGB_ALPHA:
         printf("RGBA %u %u %u %u\n", component(row, x, 0, bit_depth, 4),
            component(row, x, 1, bit_depth, 4),
            component(row, x, 2, bit_depth, 4),
            component(row, x, 3, bit_depth, 4));
 return;

 default:
         png_error(png_ptr, "pngpixel: invalid color type");
 }
}
