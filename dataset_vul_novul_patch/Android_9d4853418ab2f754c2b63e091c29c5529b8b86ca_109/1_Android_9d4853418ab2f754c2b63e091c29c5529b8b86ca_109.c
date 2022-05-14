static double outlog(PNG_CONST png_modifier *pm, int in_depth, int out_depth)
 {
    /* The command line parameters are either 8 bit (0..255) or 16 bit (0..65535)
     * and so must be adjusted for low bit depth grayscale:
    */
 if (out_depth <= 8)
 {
 if (pm->log8 == 0) /* switched off */
 return 256;

 if (out_depth < 8)
 return pm->log8 / 255 * ((1<<out_depth)-1);

 return pm->log8;
 }

 if ((pm->calculations_use_input_precision ? in_depth : out_depth) == 16)
 {
 if (pm->log16 == 0)
 return 65536;

 return pm->log16;
 }

 /* This is the case where the value was calculated at 8-bit precision then
    * scaled to 16 bits.
    */
 if (pm->log8 == 0)
 return 65536;

 return pm->log8 * 257;
}
