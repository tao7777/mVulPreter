modifier_init(png_modifier *pm)
{
   memset(pm, 0, sizeof *pm);
   store_init(&pm->this);
   pm->modifications = NULL;
   pm->state = modifier_start;
   pm->sbitlow = 1U;
   pm->ngammas = 0;
   pm->ngamma_tests = 0;
   pm->gammas = 0;
   pm->current_gamma = 0;
   pm->encodings = 0;
   pm->nencodings = 0;
   pm->current_encoding = 0;
   pm->encoding_counter = 0;
   pm->encoding_ignored = 0;
   pm->repeat = 0;
   pm->test_uses_encoding = 0;
   pm->maxout8 = pm->maxpc8 = pm->maxabs8 = pm->maxcalc8 = 0;
   pm->maxout16 = pm->maxpc16 = pm->maxabs16 = pm->maxcalc16 = 0;
   pm->maxcalcG = 0;
   pm->limit = 4E-3;
   pm->log8 = pm->log16 = 0; /* Means 'off' */
   pm->error_gray_2 = pm->error_gray_4 = pm->error_gray_8 = 0;
   pm->error_gray_16 = pm->error_color_8 = pm->error_color_16 = 0;
   pm->error_indexed = 0;
   pm->use_update_info = 0;
   pm->interlace_type = PNG_INTERLACE_NONE;

    pm->test_standard = 0;
    pm->test_size = 0;
    pm->test_transform = 0;
    pm->use_input_precision = 0;
    pm->use_input_precision_sbit = 0;
    pm->use_input_precision_16to8 = 0;
   pm->calculations_use_input_precision = 0;
   pm->assume_16_bit_calculations = 0;
   pm->test_gamma_threshold = 0;
   pm->test_gamma_transform = 0;
   pm->test_gamma_sbit = 0;
   pm->test_gamma_scale16 = 0;

    pm->test_gamma_background = 0;
    pm->test_gamma_alpha_mode = 0;
    pm->test_gamma_expand16 = 0;
    pm->test_exhaustive = 0;
    pm->log = 0;
 
 /* Rely on the memset for all the other fields - there are no pointers */
}
