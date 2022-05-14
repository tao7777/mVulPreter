static double abserr(PNG_CONST png_modifier *pm, int in_depth, int out_depth)
 {
    /* Absolute error permitted in linear values - affected by the bit depth of
     * the calculations.
    */
 if (pm->assume_16_bit_calculations ||
 (pm->calculations_use_input_precision ? in_depth : out_depth) == 16)
 return pm->maxabs16;
 else

       return pm->maxabs8;
 }
