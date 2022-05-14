interlace_row(png_bytep buffer, png_const_bytep imageRow,
   unsigned int pixel_size, png_uint_32 w, int pass)
{
   png_uint_32 xin, xout, xstep;
   /* Note that this can, trivially, be optimized to a memcpy on pass 7, the
    * code is presented this way to make it easier to understand.  In practice
    * consult the code in the libpng source to see other ways of doing this.
    */
   xin = PNG_PASS_START_COL(pass);
   xstep = 1U<<PNG_PASS_COL_SHIFT(pass);
   for (xout=0; xin<w; xin+=xstep)
   {
      pixel_copy(buffer, xout, imageRow, xin, pixel_size);
      ++xout;
   }
}
