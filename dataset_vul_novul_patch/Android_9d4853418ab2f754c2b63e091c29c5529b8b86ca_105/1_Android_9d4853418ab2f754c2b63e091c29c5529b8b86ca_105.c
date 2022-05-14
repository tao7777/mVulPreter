modifier_total_encodings(PNG_CONST png_modifier *pm)
 {
    return 1 +                 /* (1) nothing */
       pm->ngammas +           /* (2) gamma values to test */
      pm->nencodings + /* (3) total number of encodings */
 /* The following test only works after the first time through the
       * png_modifier code because 'bit_depth' is set when the IHDR is read.
       * modifier_reset, below, preserves the setting until after it has called
       * the iterate function (also below.)
       *
       * For this reason do not rely on this function outside a call to
       * modifier_reset.
       */
 ((pm->bit_depth == 16 || pm->assume_16_bit_calculations) ?
         pm->nencodings : 0); /* (4) encodings with gamma == 1.0 */
}
