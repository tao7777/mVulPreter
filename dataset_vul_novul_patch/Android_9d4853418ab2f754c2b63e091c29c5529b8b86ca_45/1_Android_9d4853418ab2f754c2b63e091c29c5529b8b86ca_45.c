 gamma_display_init(gamma_display *dp, png_modifier *pm, png_uint_32 id,
     double file_gamma, double screen_gamma, png_byte sbit, int threshold_test,
     int use_input_precision, int scale16, int expand16,
    int do_background, PNG_CONST png_color_16 *pointer_to_the_background_color,
     double background_gamma)
 {
    /* Standard fields */
   standard_display_init(&dp->this, &pm->this, id, 0/*do_interlace*/,
       pm->use_update_info);
 
    /* Parameter fields */
   dp->pm = pm;
   dp->file_gamma = file_gamma;
   dp->screen_gamma = screen_gamma;
   dp->background_gamma = background_gamma;
   dp->sbit = sbit;
   dp->threshold_test = threshold_test;
   dp->use_input_precision = use_input_precision;
   dp->scale16 = scale16;
   dp->expand16 = expand16;
   dp->do_background = do_background;
 if (do_background && pointer_to_the_background_color != 0)
      dp->background_color = *pointer_to_the_background_color;
 else
      memset(&dp->background_color, 0, sizeof dp->background_color);

 /* Local variable fields */
   dp->maxerrout = dp->maxerrpc = dp->maxerrabs = 0;
}
