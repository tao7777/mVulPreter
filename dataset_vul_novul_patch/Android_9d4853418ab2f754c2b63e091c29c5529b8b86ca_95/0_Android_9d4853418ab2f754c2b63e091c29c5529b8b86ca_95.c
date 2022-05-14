make_error(png_store* volatile psIn, png_byte PNG_CONST colour_type,
make_error(png_store* const ps, png_byte const colour_type,
     png_byte bit_depth, int interlace_type, int test, png_const_charp name)
 {
    context(ps, fault);
 
    check_interlace_type(interlace_type);
 
    Try
    {
       png_infop pi;
      const png_structp pp = set_store_for_write(ps, &pi, name);
      png_uint_32 w, h;
      gnu_volatile(pp)
 
       if (pp == NULL)
          Throw ps;
 
      w = transform_width(pp, colour_type, bit_depth);
      gnu_volatile(w)
      h = transform_height(pp, colour_type, bit_depth);
      gnu_volatile(h)
      png_set_IHDR(pp, pi, w, h, bit_depth, colour_type, interlace_type,
            PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
 
       if (colour_type == 3) /* palette */
          init_standard_palette(ps, pp, pi, 1U << bit_depth, 0/*do tRNS*/);

 /* Time for a few errors; these are in various optional chunks, the
       * standard tests test the standard chunks pretty well.
       */
#     define exception__prev exception_prev_1

 #     define exception__env exception_env_1
       Try
       {
         gnu_volatile(exception__prev)

          /* Expect this to throw: */
          ps->expect_error = !error_test[test].warning;
          ps->expect_warning = error_test[test].warning;
         ps->saw_warning = 0;
         error_test[test].fn(pp, pi);

 /* Normally the error is only detected here: */
         png_write_info(pp, pi);

 /* And handle the case where it was only a warning: */
 if (ps->expect_warning && ps->saw_warning)
 Throw ps;

 /* If we get here there is a problem, we have success - no error or
          * no warning - when we shouldn't have success.  Log an error.
          */
         store_log(ps, pp, error_test[test].msg, 1 /*error*/);

       }
 
       Catch (fault)
      { /* expected exit */
      }
 #undef exception__prev
 #undef exception__env
 
 /* And clear these flags */
      ps->expect_error = 0;
      ps->expect_warning = 0;

 /* Now write the whole image, just to make sure that the detected, or
       * undetected, errro has not created problems inside libpng.
       */
 if (png_get_rowbytes(pp, pi) !=
          transform_rowsize(pp, colour_type, bit_depth))
         png_error(pp, "row size incorrect");

 
       else
       {
         int npasses = set_write_interlace_handling(pp, interlace_type);
          int pass;
 
          if (npasses != npasses_from_interlace_type(pp, interlace_type))
            png_error(pp, "write: png_set_interlace_handling failed");

 for (pass=0; pass<npasses; ++pass)
 {
            png_uint_32 y;

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

      png_write_end(pp, pi);

 /* The following deletes the file that was just written. */
      store_write_reset(ps);
 }

 Catch(fault)
 {
      store_write_reset(fault);
 }

 }
