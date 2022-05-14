make_transform_image(png_store* PNG_CONST ps, png_byte PNG_CONST colour_type,
make_transform_image(png_store* const ps, png_byte const colour_type,
    png_byte const bit_depth, unsigned int palette_number,
     int interlace_type, png_const_charp name)
 {
    context(ps, fault);

   check_interlace_type(interlace_type);

 Try

    {
       png_infop pi;
       png_structp pp = set_store_for_write(ps, &pi, name);
      png_uint_32 h, w;
 
       /* In the event of a problem return control to the Catch statement below
        * to do the clean up - it is not possible to 'return' directly from a Try
       * block.
       */

       if (pp == NULL)
          Throw ps;
 
      w = transform_width(pp, colour_type, bit_depth);
       h = transform_height(pp, colour_type, bit_depth);
 
      png_set_IHDR(pp, pi, w, h, bit_depth, colour_type, interlace_type,
          PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
 
 #ifdef PNG_TEXT_SUPPORTED
#  if defined(PNG_READ_zTXt_SUPPORTED) && defined(PNG_WRITE_zTXt_SUPPORTED)
#     define TEXT_COMPRESSION PNG_TEXT_COMPRESSION_zTXt
#  else
#     define TEXT_COMPRESSION PNG_TEXT_COMPRESSION_NONE
#  endif
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
          init_standard_palette(ps, pp, pi, 1U << bit_depth, 1/*do tRNS*/);
 
#     ifdef PNG_WRITE_tRNS_SUPPORTED
         else if (palette_number)
            set_random_tRNS(pp, pi, colour_type, bit_depth);
#     endif

       png_write_info(pp, pi);
 
       if (png_get_rowbytes(pp, pi) !=
           transform_rowsize(pp, colour_type, bit_depth))
         png_error(pp, "transform row size incorrect");
 
       else
       {
 /* Somewhat confusingly this must be called *after* png_write_info

           * because if it is called before, the information in *pp has not been
           * updated to reflect the interlaced image.
           */
         int npasses = set_write_interlace_handling(pp, interlace_type);
          int pass;
 
          if (npasses != npasses_from_interlace_type(pp, interlace_type))
            png_error(pp, "write: png_set_interlace_handling failed");

 for (pass=0; pass<npasses; ++pass)

          {
             png_uint_32 y;
 
            /* do_own_interlace is a pre-defined boolean (a #define) which is
             * set if we have to work out the interlaced rows here.
             */
             for (y=0; y<h; ++y)
             {
                png_byte buffer[TRANSFORM_ROWMAX];
 
                transform_row(pp, buffer, colour_type, bit_depth, y);

#              if do_own_interlace
                  /* If do_own_interlace *and* the image is interlaced we need a
                   * reduced interlace row; this may be reduced to empty.
                   */
                  if (interlace_type == PNG_INTERLACE_ADAM7)
                  {
                     /* The row must not be written if it doesn't exist, notice
                      * that there are two conditions here, either the row isn't
                      * ever in the pass or the row would be but isn't wide
                      * enough to contribute any pixels.  In fact the wPass test
                      * can be used to skip the whole y loop in this case.
                      */
                     if (PNG_ROW_IN_INTERLACE_PASS(y, pass) &&
                         PNG_PASS_COLS(w, pass) > 0)
                        interlace_row(buffer, buffer,
                              bit_size(pp, colour_type, bit_depth), w, pass,
                              0/*data always bigendian*/);
                     else
                        continue;
                  }
#              endif /* do_own_interlace */

                png_write_row(pp, buffer);
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
      store_storefile(ps, FILEID(colour_type, bit_depth, palette_number,
         interlace_type, 0, 0, 0));

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
