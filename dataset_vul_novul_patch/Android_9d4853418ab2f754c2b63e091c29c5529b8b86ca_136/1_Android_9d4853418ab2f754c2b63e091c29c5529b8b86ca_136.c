standard_test(png_store* PNG_CONST psIn, png_uint_32 PNG_CONST id,
    int do_interlace, int use_update_info)
 {
    standard_display d;
   context(psIn, fault);

 /* Set up the display (stack frame) variables from the arguments to the
    * function and initialize the locals that are filled in later.
    */
   standard_display_init(&d, psIn, id, do_interlace, use_update_info);

 /* Everything is protected by a Try/Catch.  The functions called also
    * typically have local Try/Catch blocks.
    */
 Try
 {
      png_structp pp;
      png_infop pi;

 /* Get a png_struct for reading the image. This will throw an error if it
       * fails, so we don't need to check the result.
       */
      pp = set_store_for_read(d.ps, &pi, d.id,
         d.do_interlace ? (d.ps->progressive ?
 "pngvalid progressive deinterlacer" :
 "pngvalid sequential deinterlacer") : (d.ps->progressive ?
 "progressive reader" : "sequential reader"));

 /* Initialize the palette correctly from the png_store_file. */
      standard_palette_init(&d);

 /* Introduce the correct read function. */
 if (d.ps->progressive)
 {
         png_set_progressive_read_fn(pp, &d, standard_info, progressive_row,
            standard_end);

 /* Now feed data into the reader until we reach the end: */
         store_progressive_read(d.ps, pp, pi);
 }
 else
 {
 /* Note that this takes the store, not the display. */
         png_set_read_fn(pp, d.ps, store_read);

 /* Check the header values: */
         png_read_info(pp, pi);

 /* The code tests both versions of the images that the sequential
          * reader can produce.
          */
         standard_info_imp(&d, pp, pi, 2 /*images*/);

 /* Need the total bytes in the image below; we can't get to this point
          * unless the PNG file values have been checked against the expected
          * values.
          */
 {
            sequential_row(&d, pp, pi, 0, 1);

 /* After the last pass loop over the rows again to check that the
             * image is correct.
             */
 if (!d.speed)
 {
               standard_text_validate(&d, pp, pi, 1/*check_end*/);
               standard_image_validate(&d, pp, 0, 1);
 }
 else
               d.ps->validated = 1;
 }
 }

 /* Check for validation. */
 if (!d.ps->validated)
         png_error(pp, "image read failed silently");

 /* Successful completion. */
 }

 Catch(fault)
      d.ps = fault; /* make sure this hasn't been clobbered. */

 /* In either case clean up the store. */
   store_read_reset(d.ps);

 }
