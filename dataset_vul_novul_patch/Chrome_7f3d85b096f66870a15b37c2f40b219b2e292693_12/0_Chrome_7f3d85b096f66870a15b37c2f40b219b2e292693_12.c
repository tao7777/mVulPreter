 png_get_uint_16(png_bytep buf)
 {
   png_uint_16 i = ((png_uint_16)((*(buf     )) & 0xff) << 8) +
                   ((png_uint_16)((*(buf +  1)) & 0xff)     );
 
    return (i);
 }
