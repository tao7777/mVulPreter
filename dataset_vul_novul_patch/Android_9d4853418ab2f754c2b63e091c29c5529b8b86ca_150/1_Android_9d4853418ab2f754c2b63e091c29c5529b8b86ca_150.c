 transform_range_check(png_const_structp pp, unsigned int r, unsigned int g,
    unsigned int b, unsigned int a, unsigned int in_digitized, double in,
    unsigned int out, png_byte sample_depth, double err, double limit,
   PNG_CONST char *name, double digitization_error)
 {
    /* Compare the scaled, digitzed, values of our local calculation (in+-err)
     * with the digitized values libpng produced;  'sample_depth' is the actual
    * digitization depth of the libpng output colors (the bit depth except for
    * palette images where it is always 8.)  The check on 'err' is to detect
    * internal errors in pngvalid itself.
    */
 unsigned int max = (1U<<sample_depth)-1;
 double in_min = ceil((in-err)*max - digitization_error);
 double in_max = floor((in+err)*max + digitization_error);
 if (err > limit || !(out >= in_min && out <= in_max))
 {
 char message[256];
 size_t pos;

      pos = safecat(message, sizeof message, 0, name);
      pos = safecat(message, sizeof message, pos, " output value error: rgba(");
      pos = safecatn(message, sizeof message, pos, r);
      pos = safecat(message, sizeof message, pos, ",");
      pos = safecatn(message, sizeof message, pos, g);
      pos = safecat(message, sizeof message, pos, ",");
      pos = safecatn(message, sizeof message, pos, b);
      pos = safecat(message, sizeof message, pos, ",");
      pos = safecatn(message, sizeof message, pos, a);
      pos = safecat(message, sizeof message, pos, "): ");
      pos = safecatn(message, sizeof message, pos, out);
      pos = safecat(message, sizeof message, pos, " expected: ");
      pos = safecatn(message, sizeof message, pos, in_digitized);
      pos = safecat(message, sizeof message, pos, " (");
      pos = safecatd(message, sizeof message, pos, (in-err)*max, 3);
      pos = safecat(message, sizeof message, pos, "..");
      pos = safecatd(message, sizeof message, pos, (in+err)*max, 3);
      pos = safecat(message, sizeof message, pos, ")");

      png_error(pp, message);
 }
}
