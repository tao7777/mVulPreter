 png_get_int_32(png_bytep buf)
 {
   png_int_32 i = ((png_int_32)((*(buf    )) & 0xff) << 24) +
                  ((png_int_32)((*(buf + 1)) & 0xff) << 16) +
                  ((png_int_32)((*(buf + 2)) & 0xff) <<  8) +
                  ((png_int_32)((*(buf + 3)) & 0xff)      );
 
    return (i);
 }
