standard_row_validate(standard_display *dp, png_const_structp pp,
 int iImage, int iDisplay, png_uint_32 y)
{
 int where;
   png_byte std[STANDARD_ROWMAX];

 /* The row must be pre-initialized to the magic number here for the size
    * tests to pass:
    */
   memset(std, 178, sizeof std);
   standard_row(pp, std, dp->id, y);

 /* At the end both the 'row' and 'display' arrays should end up identical.

     * In earlier passes 'row' will be partially filled in, with only the pixels
     * that have been read so far, but 'display' will have those pixels
     * replicated to fill the unread pixels while reading an interlaced image.
     */
    if (iImage >= 0 &&
       (where = pixel_cmp(std, store_image_row(dp->ps, pp, iImage, y),
            dp->bit_width)) != 0)
 {
 char msg[64];
      sprintf(msg, "PNG image row[%lu][%d] changed from %.2x to %.2x",
 (unsigned long)y, where-1, std[where-1],
         store_image_row(dp->ps, pp, iImage, y)[where-1]);

       png_error(pp, msg);
    }
 
    if (iDisplay >= 0 &&
       (where = pixel_cmp(std, store_image_row(dp->ps, pp, iDisplay, y),
          dp->bit_width)) != 0)
    {
       char msg[64];
      sprintf(msg, "display row[%lu][%d] changed from %.2x to %.2x",
          (unsigned long)y, where-1, std[where-1],
          store_image_row(dp->ps, pp, iDisplay, y)[where-1]);
       png_error(pp, msg);
 }
}
