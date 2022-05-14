static double calcerr(PNG_CONST png_modifier *pm, int in_depth, int out_depth)
static double calcerr(const png_modifier *pm, int in_depth, int out_depth)
 {
    /* Error in the linear composition arithmetic - only relevant when
     * composition actually happens (0 < alpha < 1).
    */
 if ((pm->calculations_use_input_precision ? in_depth : out_depth) == 16)
 return pm->maxcalc16;
 else if (pm->assume_16_bit_calculations)
 return pm->maxcalcG;
 else

       return pm->maxcalc8;
 }
