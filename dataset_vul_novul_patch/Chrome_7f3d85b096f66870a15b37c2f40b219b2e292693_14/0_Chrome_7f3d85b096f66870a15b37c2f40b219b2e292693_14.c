 png_get_uint_32(png_bytep buf)
 {
   png_uint_32 i = ((png_uint_32)((*(buf    )) & 0xff) << 24) +
                   ((png_uint_32)((*(buf + 1)) & 0xff) << 16) +
                   ((png_uint_32)((*(buf + 2)) & 0xff) <<  8) +
                   ((png_uint_32)((*(buf + 3)) & 0xff)      );
 
    return (i);
 }
