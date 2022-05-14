png_get_uint_31(png_structp png_ptr, png_bytep buf)
{
#ifdef PNG_READ_BIG_ENDIAN_SUPPORTED
    png_uint_32 i = png_get_uint_32(buf);
 #else
    /* Avoid an extra function call by inlining the result. */
   png_uint_32 i = ((png_uint_32)((*(buf    )) & 0xff) << 24) +
                   ((png_uint_32)((*(buf + 1)) & 0xff) << 16) +
                   ((png_uint_32)((*(buf + 2)) & 0xff) <<  8) +
                   ((png_uint_32)((*(buf + 3)) & 0xff)      );
 #endif
    if (i > PNG_UINT_31_MAX)
      png_error(png_ptr, "PNG unsigned integer out of range.");
    return (i);
 }
