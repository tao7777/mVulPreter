make_size_image(png_store* PNG_CONST ps, png_byte PNG_CONST colour_type,
    png_byte PNG_CONST bit_depth, int PNG_CONST interlace_type,
    png_uint_32 PNG_CONST w, png_uint_32 PNG_CONST h,
    int PNG_CONST do_interlace)
 {
    context(ps, fault);
 
   /* At present libpng does not support the write of an interlaced image unless
    * PNG_WRITE_INTERLACING_SUPPORTED, even with do_interlace so the code here
    * does the pixel interlace itself, so:
    */
    check_interlace_type(interlace_type);
 
    Try
 {
      png_infop pi;
      png_structp pp;
 unsigned int pixel_size;

 
       /* Make a name and get an appropriate id for the store: */
       char name[FILE_NAME_SIZE];
      PNG_CONST png_uint_32 id = FILEID(colour_type, bit_depth, 0/*palette*/,
          interlace_type, w, h, do_interlace);
 
       standard_name_from_id(name, sizeof name, 0, id);
      pp = set_store_for_write(ps, &pi, name);

 /* In the event of a problem return control to the Catch statement below
       * to do the clean up - it is not possible to 'return' directly from a Try
       * block.
       */
 if (pp == NULL)
 Throw ps;

      png_set_IHDR(pp, pi, w, h, bit_depth, colour_type, interlace_type,
         PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

#ifdef PNG_TEXT_SUPPORTED
 {
 static char key[] = "image name"; /* must be writeable */
 size_t pos;
         png_text text;
 char copy[FILE_NAME_SIZE];

 /* Use a compressed text string to test the correct interaction of text
          * compression and IDAT compression.
          */
         text.compression = TEXT_COMPRESSION;
         text.key = key;
 /* Yuck: the text must be writable! */
         pos = safecat(copy, sizeof copy, 0, ps->wname);
         text.text = copy;
         text.text_length = pos;
         text.itxt_length = 0;
         text.lang = 0;
         text.lang_key = 0;

         png_set_text(pp, pi, &text, 1);
 }
#endif

 if (colour_type == 3) /* palette */
         init_standard_palette(ps, pp, pi, 1U << bit_depth, 0/*do tRNS*/);

      png_write_info(pp, pi);

 /* Calculate the bit size, divide by 8 to get the byte size - this won't
       * overflow because we know the w values are all small enough even for
       * a system where 'unsigned int' is only 16 bits.

        */
       pixel_size = bit_size(pp, colour_type, bit_depth);
       if (png_get_rowbytes(pp, pi) != ((w * pixel_size) + 7) / 8)
         png_error(pp, "row size incorrect");
 
       else
       {
 int npasses = npasses_from_interlace_type(pp, interlace_type);
         png_uint_32 y;
 int pass;
#        ifdef PNG_WRITE_FILTER_SUPPORTED
 int nfilter = PNG_FILTER_VALUE_LAST;
#        endif
         png_byte image[16][SIZE_ROWMAX];

 /* To help consistent error detection make the parts of this buffer
          * that aren't set below all '1':

           */
          memset(image, 0xff, sizeof image);
 
         if (!do_interlace && npasses != png_set_interlace_handling(pp))
             png_error(pp, "write: png_set_interlace_handling failed");
 
          /* Prepare the whole image first to avoid making it 7 times: */
 for (y=0; y<h; ++y)
            size_row(image[y], w * pixel_size, y);


          for (pass=0; pass<npasses; ++pass)
          {
             /* The following two are for checking the macros: */
            PNG_CONST png_uint_32 wPass = PNG_PASS_COLS(w, pass);
 
             /* If do_interlace is set we don't call png_write_row for every
              * row because some of them are empty.  In fact, for a 1x1 image,
             * most of them are empty!
             */
 for (y=0; y<h; ++y)
 {
               png_const_bytep row = image[y];
               png_byte tempRow[SIZE_ROWMAX];

 /* If do_interlace *and* the image is interlaced we
                * need a reduced interlace row; this may be reduced
                * to empty.
                */
 if (do_interlace && interlace_type == PNG_INTERLACE_ADAM7)
 {
 /* The row must not be written if it doesn't exist, notice
                   * that there are two conditions here, either the row isn't
                   * ever in the pass or the row would be but isn't wide
                   * enough to contribute any pixels.  In fact the wPass test
                   * can be used to skip the whole y loop in this case.
                   */
 if (PNG_ROW_IN_INTERLACE_PASS(y, pass) && wPass > 0)
 {
 /* Set to all 1's for error detection (libpng tends to

                       * set unset things to 0).
                       */
                      memset(tempRow, 0xff, sizeof tempRow);
                     interlace_row(tempRow, row, pixel_size, w, pass);
                      row = tempRow;
                   }
                   else
 continue;
 }

#           ifdef PNG_WRITE_FILTER_SUPPORTED
 /* Only get to here if the row has some pixels in it, set the
                * filters to 'all' for the very first row and thereafter to a
                * single filter.  It isn't well documented, but png_set_filter
                * does accept a filter number (per the spec) as well as a bit
                * mask.
                *
                * The apparent wackiness of decrementing nfilter rather than
                * incrementing is so that Paeth gets used in all images bigger
                * than 1 row - it's the tricky one.
                */
               png_set_filter(pp, 0/*method*/,
                  nfilter >= PNG_FILTER_VALUE_LAST ? PNG_ALL_FILTERS : nfilter);

 if (nfilter-- == 0)
                  nfilter = PNG_FILTER_VALUE_LAST-1;
#           endif

               png_write_row(pp, row);
 }
 }
 }

#ifdef PNG_TEXT_SUPPORTED
 {
 static char key[] = "end marker";
 static char comment[] = "end";
         png_text text;

 /* Use a compressed text string to test the correct interaction of text
          * compression and IDAT compression.
          */
         text.compression = TEXT_COMPRESSION;
         text.key = key;
         text.text = comment;
         text.text_length = (sizeof comment)-1;
         text.itxt_length = 0;
         text.lang = 0;
         text.lang_key = 0;

         png_set_text(pp, pi, &text, 1);
 }
#endif

      png_write_end(pp, pi);

 /* And store this under the appropriate id, then clean up. */
      store_storefile(ps, id);

      store_write_reset(ps);
 }

 Catch(fault)
 {
 /* Use the png_store returned by the exception. This may help the compiler
       * because 'ps' is not used in this branch of the setjmp.  Note that fault
       * and ps will always be the same value.
       */
      store_write_reset(fault);
 }

 }
