 png_get_uint_16(png_bytep buf)
 {
   png_uint_16 i = (png_uint_16)(((png_uint_16)(*buf) << 8) +
      (png_uint_16)(*(buf + 1)));
 
    return (i);
 }
