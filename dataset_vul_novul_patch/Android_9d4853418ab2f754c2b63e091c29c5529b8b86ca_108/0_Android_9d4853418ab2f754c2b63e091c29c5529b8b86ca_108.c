static double outerr(PNG_CONST png_modifier *pm, int in_depth, int out_depth)
static double outerr(const png_modifier *pm, int in_depth, int out_depth)
 {
    /* There is a serious error in the 2 and 4 bit grayscale transform because
     * the gamma table value (8 bits) is simply shifted, not rounded, so the
    * error in 4 bit grayscale gamma is up to the value below.  This is a hack
    * to allow pngvalid to succeed:
    *
    * TODO: fix this in libpng
    */
 if (out_depth == 2)
 return .73182-.5;

 if (out_depth == 4)
 return .90644-.5;

 if ((pm->calculations_use_input_precision ? in_depth : out_depth) == 16)
 return pm->maxout16;

 /* This is the case where the value was calculated at 8-bit precision then
    * scaled to 16 bits.
    */
 else if (out_depth == 16)
 return pm->maxout8 * 257;

 else
 return pm->maxout8;
}
