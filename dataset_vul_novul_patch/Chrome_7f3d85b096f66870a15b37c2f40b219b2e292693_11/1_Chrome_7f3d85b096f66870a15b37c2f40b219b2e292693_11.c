 png_get_int_32(png_bytep buf)
 {
   png_int_32 i = ((png_int_32)(*buf) << 24) +
      ((png_int_32)(*(buf + 1)) << 16) +
      ((png_int_32)(*(buf + 2)) << 8) +
      (png_int_32)(*(buf + 3));
 
    return (i);
 }
