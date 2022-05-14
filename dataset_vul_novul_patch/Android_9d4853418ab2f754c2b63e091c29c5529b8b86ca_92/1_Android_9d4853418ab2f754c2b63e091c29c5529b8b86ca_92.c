 init_validate_info(validate_info *vi, gamma_display *dp, png_const_structp pp,
     int in_depth, int out_depth)
 {
   PNG_CONST unsigned int outmax = (1U<<out_depth)-1;
 
    vi->pp = pp;
    vi->dp = dp;

 if (dp->sbit > 0 && dp->sbit < in_depth)
 {
      vi->sbit = dp->sbit;
      vi->isbit_shift = in_depth - dp->sbit;
 }

 else
 {
      vi->sbit = (png_byte)in_depth;
      vi->isbit_shift = 0;
 }

   vi->sbit_max = (1U << vi->sbit)-1;

 /* This mimics the libpng threshold test, '0' is used to prevent gamma
    * correction in the validation test.
    */
   vi->screen_gamma = dp->screen_gamma;
 if (fabs(vi->screen_gamma-1) < PNG_GAMMA_THRESHOLD)
      vi->screen_gamma = vi->screen_inverse = 0;
 else
      vi->screen_inverse = 1/vi->screen_gamma;

   vi->use_input_precision = dp->use_input_precision;
   vi->outmax = outmax;
   vi->maxabs = abserr(dp->pm, in_depth, out_depth);
   vi->maxpc = pcerr(dp->pm, in_depth, out_depth);
   vi->maxcalc = calcerr(dp->pm, in_depth, out_depth);
   vi->maxout = outerr(dp->pm, in_depth, out_depth);
   vi->outquant = output_quantization_factor(dp->pm, in_depth, out_depth);
   vi->maxout_total = vi->maxout + vi->outquant * .5;

    vi->outlog = outlog(dp->pm, in_depth, out_depth);
 
    if ((dp->this.colour_type & PNG_COLOR_MASK_ALPHA) != 0 ||
      (dp->this.colour_type == 3 && dp->this.is_transparent))
    {
       vi->do_background = dp->do_background;
 
       if (vi->do_background != 0)
       {
         PNG_CONST double bg_inverse = 1/dp->background_gamma;
          double r, g, b;
 
          /* Caller must at least put the gray value into the red channel */
         r = dp->background_color.red; r /= outmax;
         g = dp->background_color.green; g /= outmax;
         b = dp->background_color.blue; b /= outmax;

#     if 0
 /* libpng doesn't do this optimization, if we do pngvalid will fail.
          */
 if (fabs(bg_inverse-1) >= PNG_GAMMA_THRESHOLD)
#     endif
 {
            r = pow(r, bg_inverse);
            g = pow(g, bg_inverse);
            b = pow(b, bg_inverse);
 }

         vi->background_red = r;
         vi->background_green = g;

          vi->background_blue = b;
       }
    }
   else
       vi->do_background = 0;
 
    if (vi->do_background == 0)
      vi->background_red = vi->background_green = vi->background_blue = 0;

   vi->gamma_correction = 1/(dp->file_gamma*dp->screen_gamma);
 if (fabs(vi->gamma_correction-1) < PNG_GAMMA_THRESHOLD)
      vi->gamma_correction = 0;

   vi->file_inverse = 1/dp->file_gamma;
 if (fabs(vi->file_inverse-1) < PNG_GAMMA_THRESHOLD)
      vi->file_inverse = 0;

   vi->scale16 = dp->scale16;
}
