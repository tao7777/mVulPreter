gamma_test(png_modifier *pmIn, PNG_CONST png_byte colour_typeIn,
    PNG_CONST png_byte bit_depthIn, PNG_CONST int palette_numberIn,
    PNG_CONST int interlace_typeIn,
    PNG_CONST double file_gammaIn, PNG_CONST double screen_gammaIn,
    PNG_CONST png_byte sbitIn, PNG_CONST int threshold_testIn,
    PNG_CONST char *name,
    PNG_CONST int use_input_precisionIn, PNG_CONST int scale16In,
    PNG_CONST int expand16In, PNG_CONST int do_backgroundIn,
    PNG_CONST png_color_16 *bkgd_colorIn, double bkgd_gammaIn)
 {
    gamma_display d;
    context(&pmIn->this, fault);

   gamma_display_init(&d, pmIn, FILEID(colour_typeIn, bit_depthIn,
      palette_numberIn, interlace_typeIn, 0, 0, 0),
      file_gammaIn, screen_gammaIn, sbitIn,
      threshold_testIn, use_input_precisionIn, scale16In,
      expand16In, do_backgroundIn, bkgd_colorIn, bkgd_gammaIn);

 Try
 {
      png_structp pp;
      png_infop pi;
      gama_modification gama_mod;
      srgb_modification srgb_mod;
      sbit_modification sbit_mod;

 /* For the moment don't use the png_modifier support here. */
      d.pm->encoding_counter = 0;
      modifier_set_encoding(d.pm); /* Just resets everything */
      d.pm->current_gamma = d.file_gamma;

 /* Make an appropriate modifier to set the PNG file gamma to the
       * given gamma value and the sBIT chunk to the given precision.
       */
      d.pm->modifications = NULL;
      gama_modification_init(&gama_mod, d.pm, d.file_gamma);
      srgb_modification_init(&srgb_mod, d.pm, 127 /*delete*/);
 if (d.sbit > 0)
         sbit_modification_init(&sbit_mod, d.pm, d.sbit);

 
       modification_reset(d.pm->modifications);
 
      /* Get a png_struct for writing the image. */
       pp = set_modifier_for_read(d.pm, &pi, d.this.id, name);
       standard_palette_init(&d.this);
 
 /* Introduce the correct read function. */
 if (d.pm->this.progressive)
 {
 /* Share the row function with the standard implementation. */
         png_set_progressive_read_fn(pp, &d, gamma_info, progressive_row,
            gamma_end);

 /* Now feed data into the reader until we reach the end: */
         modifier_progressive_read(d.pm, pp, pi);
 }
 else
 {
 /* modifier_read expects a png_modifier* */
         png_set_read_fn(pp, d.pm, modifier_read);

 /* Check the header values: */
         png_read_info(pp, pi);

 /* Process the 'info' requirements. Only one image is generated */
         gamma_info_imp(&d, pp, pi);

         sequential_row(&d.this, pp, pi, -1, 0);

 if (!d.this.speed)
            gamma_image_validate(&d, pp, pi);
 else
            d.this.ps->validated = 1;
 }

      modifier_reset(d.pm);

 if (d.pm->log && !d.threshold_test && !d.this.speed)
         fprintf(stderr, "%d bit %s %s: max error %f (%.2g, %2g%%)\n",
            d.this.bit_depth, colour_types[d.this.colour_type], name,
            d.maxerrout, d.maxerrabs, 100*d.maxerrpc);

 /* Log the summary values too. */
 if (d.this.colour_type == 0 || d.this.colour_type == 4)
 {
 switch (d.this.bit_depth)
 {
 case 1:
 break;

 case 2:
 if (d.maxerrout > d.pm->error_gray_2)
               d.pm->error_gray_2 = d.maxerrout;

 break;

 case 4:
 if (d.maxerrout > d.pm->error_gray_4)
               d.pm->error_gray_4 = d.maxerrout;

 break;

 case 8:
 if (d.maxerrout > d.pm->error_gray_8)
               d.pm->error_gray_8 = d.maxerrout;

 break;

 case 16:
 if (d.maxerrout > d.pm->error_gray_16)
               d.pm->error_gray_16 = d.maxerrout;

 break;

 default:
            png_error(pp, "bad bit depth (internal: 1)");
 }
 }

 else if (d.this.colour_type == 2 || d.this.colour_type == 6)
 {
 switch (d.this.bit_depth)
 {
 case 8:

 if (d.maxerrout > d.pm->error_color_8)
               d.pm->error_color_8 = d.maxerrout;

 break;

 case 16:

 if (d.maxerrout > d.pm->error_color_16)
               d.pm->error_color_16 = d.maxerrout;

 break;

 default:
            png_error(pp, "bad bit depth (internal: 2)");
 }
 }

 else if (d.this.colour_type == 3)
 {
 if (d.maxerrout > d.pm->error_indexed)
            d.pm->error_indexed = d.maxerrout;
 }
 }

 Catch(fault)
      modifier_reset(voidcast(png_modifier*,(void*)fault));
}
