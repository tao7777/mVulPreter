standard_name(char *buffer, size_t bufsize, size_t pos, png_byte colour_type,
 int bit_depth, unsigned int npalette, int interlace_type,

     png_uint_32 w, png_uint_32 h, int do_interlace)
 {
    pos = safecat(buffer, bufsize, pos, colour_types[colour_type]);
   if (npalette > 0)
    {
       pos = safecat(buffer, bufsize, pos, "[");
       pos = safecatn(buffer, bufsize, pos, npalette);
       pos = safecat(buffer, bufsize, pos, "]");
    }
    pos = safecat(buffer, bufsize, pos, " ");
    pos = safecatn(buffer, bufsize, pos, bit_depth);
    pos = safecat(buffer, bufsize, pos, " bit");

 if (interlace_type != PNG_INTERLACE_NONE)
 {
      pos = safecat(buffer, bufsize, pos, " interlaced");
 if (do_interlace)
         pos = safecat(buffer, bufsize, pos, "(pngvalid)");
 else
         pos = safecat(buffer, bufsize, pos, "(libpng)");
 }

 if (w > 0 || h > 0)
 {
      pos = safecat(buffer, bufsize, pos, " ");
      pos = safecatn(buffer, bufsize, pos, w);
      pos = safecat(buffer, bufsize, pos, "x");
      pos = safecatn(buffer, bufsize, pos, h);
 }

 return pos;
}
