perform_gamma_threshold_tests(png_modifier *pm)
{
   png_byte colour_type = 0;
   png_byte bit_depth = 0;
 unsigned int palette_number = 0;


    /* Don't test more than one instance of each palette - it's pointless, in
     * fact this test is somewhat excessive since libpng doesn't make this
     * decision based on colour type or bit depth!
    *
    * CHANGED: now test two palettes and, as a side effect, images with and
    * without tRNS.
     */
   while (next_format(&colour_type, &bit_depth, &palette_number,
                      pm->test_lbg_gamma_threshold, pm->test_tRNS))
      if (palette_number < 2)
    {
       double test_gamma = 1.0;
       while (test_gamma >= .4)
 {
 /* There's little point testing the interlacing vs non-interlacing,
          * but this can be set from the command line.
          */
         gamma_threshold_test(pm, colour_type, bit_depth, pm->interlace_type,
            test_gamma, 1/test_gamma);
         test_gamma *= .95;
 }

 /* And a special test for sRGB */
      gamma_threshold_test(pm, colour_type, bit_depth, pm->interlace_type,
 .45455, 2.2);

 if (fail(pm))
 return;
 }

 }
