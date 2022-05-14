 safecat_current_encoding(char *buffer, size_t bufsize, size_t pos,
   const png_modifier *pm)
 {
    pos = safecat_color_encoding(buffer, bufsize, pos, pm->current_encoding,
       pm->current_gamma);

 if (pm->encoding_ignored)
      pos = safecat(buffer, bufsize, pos, "[overridden]");

 
    return pos;
 }
