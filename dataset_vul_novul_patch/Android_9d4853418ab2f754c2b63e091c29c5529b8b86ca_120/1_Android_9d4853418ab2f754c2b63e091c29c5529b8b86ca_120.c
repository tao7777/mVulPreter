 progressive_row(png_structp ppIn, png_bytep new_row, png_uint_32 y, int pass)
 {
    png_const_structp pp = ppIn;
   PNG_CONST standard_display *dp = voidcast(standard_display*,
       png_get_progressive_ptr(pp));
 
    /* When handling interlacing some rows will be absent in each pass, the
    * callback still gets called, but with a NULL pointer.  This is checked
    * in the 'else' clause below.  We need our own 'cbRow', but we can't call
    * png_get_rowbytes because we got no info structure.
    */
 if (new_row != NULL)
 {
      png_bytep row;

 /* In the case where the reader doesn't do the interlace it gives
       * us the y in the sub-image:
       */
 if (dp->do_interlace && dp->interlace_type == PNG_INTERLACE_ADAM7)
 {
#ifdef PNG_USER_TRANSFORM_INFO_SUPPORTED
 /* Use this opportunity to validate the png 'current' APIs: */
 if (y != png_get_current_row_number(pp))
            png_error(pp, "png_get_current_row_number is broken");

 
          if (pass != png_get_current_pass_number(pp))
             png_error(pp, "png_get_current_pass_number is broken");
#endif
 
          y = PNG_ROW_FROM_PASS_ROW(y, pass);
       }

 /* Validate this just in case. */
 if (y >= dp->h)
         png_error(pp, "invalid y to progressive row callback");

 
       row = store_image_row(dp->ps, pp, 0, y);
 
#ifdef PNG_READ_INTERLACING_SUPPORTED
       /* Combine the new row into the old: */
       if (dp->do_interlace)
       {
          if (dp->interlace_type == PNG_INTERLACE_ADAM7)
            deinterlace_row(row, new_row, dp->pixel_size, dp->w, pass);
          else
            row_copy(row, new_row, dp->pixel_size * dp->w);
       }
       else
          png_progressive_combine_row(pp, row, new_row);
 #endif /* PNG_READ_INTERLACING_SUPPORTED */
    }
 
#ifdef PNG_READ_INTERLACING_SUPPORTED
    else if (dp->interlace_type == PNG_INTERLACE_ADAM7 &&
        PNG_ROW_IN_INTERLACE_PASS(y, pass) &&
        PNG_PASS_COLS(dp->w, pass) > 0)
       png_error(pp, "missing row in progressive de-interlacing");
#endif /* PNG_READ_INTERLACING_SUPPORTED */
 }
