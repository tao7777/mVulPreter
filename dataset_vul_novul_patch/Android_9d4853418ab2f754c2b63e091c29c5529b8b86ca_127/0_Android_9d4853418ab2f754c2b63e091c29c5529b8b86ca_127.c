 sequential_row(standard_display *dp, png_structp pp, png_infop pi,
    const int iImage, const int iDisplay)
 {
   const int         npasses = dp->npasses;
   const int         do_interlace = dp->do_interlace &&
       dp->interlace_type == PNG_INTERLACE_ADAM7;
   const png_uint_32 height = standard_height(pp, dp->id);
   const png_uint_32 width = standard_width(pp, dp->id);
   const png_store*  ps = dp->ps;
    int pass;
 
    for (pass=0; pass<npasses; ++pass)
 {
      png_uint_32 y;
      png_uint_32 wPass = PNG_PASS_COLS(width, pass);

 for (y=0; y<height; ++y)
 {
 if (do_interlace)
 {
 /* wPass may be zero or this row may not be in this pass.
             * png_read_row must not be called in either case.
             */
 if (wPass > 0 && PNG_ROW_IN_INTERLACE_PASS(y, pass))
 {
 /* Read the row into a pair of temporary buffers, then do the
                * merge here into the output rows.
                */
               png_byte row[STANDARD_ROWMAX], display[STANDARD_ROWMAX];

 /* The following aids (to some extent) error detection - we can
                * see where png_read_row wrote.  Use opposite values in row and
                * display to make this easier.  Don't use 0xff (which is used in
                * the image write code to fill unused bits) or 0 (which is a
                * likely value to overwrite unused bits with).
                */
               memset(row, 0xc5, sizeof row);
               memset(display, 0x5c, sizeof display);

               png_read_row(pp, row, display);

 
                if (iImage >= 0)
                   deinterlace_row(store_image_row(ps, pp, iImage, y), row,
                     dp->pixel_size, dp->w, pass, dp->littleendian);
 
                if (iDisplay >= 0)
                   deinterlace_row(store_image_row(ps, pp, iDisplay, y), display,
                     dp->pixel_size, dp->w, pass, dp->littleendian);
             }
          }
          else
            png_read_row(pp,
               iImage >= 0 ? store_image_row(ps, pp, iImage, y) : NULL,
               iDisplay >= 0 ? store_image_row(ps, pp, iDisplay, y) : NULL);
 }
 }

 /* And finish the read operation (only really necessary if the caller wants
    * to find additional data in png_info from chunks after the last IDAT.)
    */
   png_read_end(pp, pi);
}
