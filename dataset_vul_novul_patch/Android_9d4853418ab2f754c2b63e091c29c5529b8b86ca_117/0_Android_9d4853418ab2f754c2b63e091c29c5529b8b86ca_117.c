static void perform_gamma_transform_tests(png_modifier *pm)
{
   png_byte colour_type = 0;

    png_byte bit_depth = 0;
    unsigned int palette_number = 0;
 
   while (next_format(&colour_type, &bit_depth, &palette_number,
                      pm->test_lbg_gamma_transform, pm->test_tRNS))
    {
       unsigned int i, j;
 
 for (i=0; i<pm->ngamma_tests; ++i) for (j=0; j<pm->ngamma_tests; ++j)
 if (i != j)
 {
            gamma_transform_test(pm, colour_type, bit_depth, palette_number,
               pm->interlace_type, 1/pm->gammas[i], pm->gammas[j], 0/*sBIT*/,
               pm->use_input_precision, 0 /*do not scale16*/);

 if (fail(pm))
 return;
 }
 }
}
