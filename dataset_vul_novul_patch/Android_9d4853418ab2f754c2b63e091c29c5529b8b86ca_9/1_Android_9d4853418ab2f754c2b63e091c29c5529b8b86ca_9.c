static void rpng2_x_redisplay_image(ulg startcol, ulg startrow,
                                    ulg width, ulg height)
{
    uch bg_red   = rpng2_info.bg_red;
    uch bg_green = rpng2_info.bg_green;
    uch bg_blue  = rpng2_info.bg_blue;
    uch *src, *src2=NULL;
 char *dest;
    uch r, g, b, a;
    ulg i, row, lastrow = 0;
    ulg pixel;
 int ximage_rowbytes = ximage->bytes_per_line;


 Trace((stderr, "beginning display loop (image_channels == %d)\n",
      rpng2_info.channels))
 Trace((stderr, "   (width = %ld, rowbytes = %d, ximage_rowbytes = %d)\n",
      rpng2_info.width, rpng2_info.rowbytes, ximage_rowbytes))
 Trace((stderr, "   (bpp = %d)\n", ximage->bits_per_pixel))
 Trace((stderr, "   (byte_order = %s)\n", ximage->byte_order == MSBFirst?
 "MSBFirst" : (ximage->byte_order == LSBFirst? "LSBFirst" : "unknown")))

/*---------------------------------------------------------------------------
    Aside from the use of the rpng2_info struct and of src2 (for background
    image), this routine is identical to rpng_x_display_image() in the non-
    progressive version of the program--for the simple reason that redisplay
    of the image against a new background happens after the image is fully
    decoded and therefore is, by definition, non-progressive.
  ---------------------------------------------------------------------------*/

 if (depth == 24 || depth == 32) {
        ulg red, green, blue;
 int bpp = ximage->bits_per_pixel;

 for (lastrow = row = startrow;  row < startrow+height; ++row) {
            src = rpng2_info.image_data + row*rpng2_info.rowbytes;
 if (bg_image)
                src2 = bg_data + row*bg_rowbytes;
            dest = ximage->data + row*ximage_rowbytes;
 if (rpng2_info.channels == 3) {
 for (i = rpng2_info.width;  i > 0; --i) {
                    red   = *src++;
                    green = *src++;
                    blue  = *src++;
#ifdef NO_24BIT_MASKS
                    pixel = (red   << RShift) |
 (green << GShift) |
 (blue  << BShift);
 /* recall that we set ximage->byte_order = MSBFirst above */
 if (bpp == 32) {
 *dest++ = (char)((pixel >> 24) & 0xff);
 *dest++ = (char)((pixel >> 16) & 0xff);
 *dest++ = (char)((pixel >> 8) & 0xff);
 *dest++ = (char)( pixel        & 0xff);
 } else {
 /* this assumes bpp == 24 & bits are packed low */
 /* (probably need to use RShift, RMask, etc.) */
 *dest++ = (char)((pixel >> 16) & 0xff);
 *dest++ = (char)((pixel >> 8) & 0xff);
 *dest++ = (char)( pixel        & 0xff);
 }
#else
                    red   = (RShift < 0)? red   << (-RShift) : red   >> RShift;
                    green = (GShift < 0)? green << (-GShift) : green >> GShift;
                    blue  = (BShift < 0)? blue  << (-BShift) : blue  >> BShift;
                    pixel = (red & RMask) | (green & GMask) | (blue & BMask);
 /* recall that we set ximage->byte_order = MSBFirst above */
 if (bpp == 32) {
 *dest++ = (char)((pixel >> 24) & 0xff);
 *dest++ = (char)((pixel >> 16) & 0xff);
 *dest++ = (char)((pixel >> 8) & 0xff);
 *dest++ = (char)( pixel        & 0xff);
 } else {
 /* GRR BUG */
 /* this assumes bpp == 24 & bits are packed low */
 /* (probably need to use RShift/RMask/etc. here, too) */
 *dest++ = (char)((pixel >> 16) & 0xff);
 *dest++ = (char)((pixel >> 8) & 0xff);
 *dest++ = (char)( pixel        & 0xff);
 }
#endif
 }

 } else /* if (rpng2_info.channels == 4) */ {
 for (i = rpng2_info.width;  i > 0; --i) {
                    r = *src++;
                    g = *src++;
                    b = *src++;
                    a = *src++;
 if (bg_image) {
                        bg_red   = *src2++;
                        bg_green = *src2++;
                        bg_blue  = *src2++;
 }
 if (a == 255) {
                        red   = r;
                        green = g;
                        blue  = b;
 } else if (a == 0) {
                        red   = bg_red;
                        green = bg_green;
                        blue  = bg_blue;
 } else {
 /* this macro (from png.h) composites the foreground
                         * and background values and puts the result into the
                         * first argument */
                        alpha_composite(red,   r, a, bg_red);
                        alpha_composite(green, g, a, bg_green);
                        alpha_composite(blue,  b, a, bg_blue);
 }
#ifdef NO_24BIT_MASKS
                    pixel = (red   << RShift) |
 (green << GShift) |
 (blue  << BShift);
 /* recall that we set ximage->byte_order = MSBFirst above */
 if (bpp == 32) {
 *dest++ = (char)((pixel >> 24) & 0xff);
 *dest++ = (char)((pixel >> 16) & 0xff);
 *dest++ = (char)((pixel >> 8) & 0xff);
 *dest++ = (char)( pixel        & 0xff);
 } else {
 /* this assumes bpp == 24 & bits are packed low */
 /* (probably need to use RShift, RMask, etc.) */
 *dest++ = (char)((pixel >> 16) & 0xff);
 *dest++ = (char)((pixel >> 8) & 0xff);
 *dest++ = (char)( pixel        & 0xff);
 }
#else
                    red   = (RShift < 0)? red   << (-RShift) : red   >> RShift;
                    green = (GShift < 0)? green << (-GShift) : green >> GShift;
                    blue  = (BShift < 0)? blue  << (-BShift) : blue  >> BShift;
                    pixel = (red & RMask) | (green & GMask) | (blue & BMask);
 /* recall that we set ximage->byte_order = MSBFirst above */
 if (bpp == 32) {
 *dest++ = (char)((pixel >> 24) & 0xff);
 *dest++ = (char)((pixel >> 16) & 0xff);
 *dest++ = (char)((pixel >> 8) & 0xff);
 *dest++ = (char)( pixel        & 0xff);
 } else {
 /* GRR BUG */
 /* this assumes bpp == 24 & bits are packed low */
 /* (probably need to use RShift/RMask/etc. here, too) */
 *dest++ = (char)((pixel >> 16) & 0xff);
 *dest++ = (char)((pixel >> 8) & 0xff);
 *dest++ = (char)( pixel        & 0xff);
 }
#endif
 }
 }
 /* display after every 16 lines */
 if (((row+1) & 0xf) == 0) {
 XPutImage(display, window, gc, ximage, 0, (int)lastrow, 0,
 (int)lastrow, rpng2_info.width, 16);
 XFlush(display);
                lastrow = row + 1;
 }
 }

 } else if (depth == 16) {
        ush red, green, blue;

 for (lastrow = row = startrow;  row < startrow+height; ++row) {
            src = rpng2_info.row_pointers[row];
 if (bg_image)
                src2 = bg_data + row*bg_rowbytes;
            dest = ximage->data + row*ximage_rowbytes;
 if (rpng2_info.channels == 3) {
 for (i = rpng2_info.width;  i > 0; --i) {
                    red   = ((ush)(*src) << 8);
 ++src;
                    green = ((ush)(*src) << 8);
 ++src;
                    blue  = ((ush)(*src) << 8);
 ++src;
                    pixel = ((red   >> RShift) & RMask) |
 ((green >> GShift) & GMask) |
 ((blue  >> BShift) & BMask);
 /* recall that we set ximage->byte_order = MSBFirst above */
 *dest++ = (char)((pixel >> 8) & 0xff);
 *dest++ = (char)( pixel        & 0xff);
 }
 } else /* if (rpng2_info.channels == 4) */ {
 for (i = rpng2_info.width;  i > 0; --i) {
                    r = *src++;
                    g = *src++;
                    b = *src++;
                    a = *src++;
 if (bg_image) {
                        bg_red   = *src2++;
                        bg_green = *src2++;
                        bg_blue  = *src2++;
 }
 if (a == 255) {
                        red   = ((ush)r << 8);
                        green = ((ush)g << 8);
                        blue  = ((ush)b << 8);
 } else if (a == 0) {
                        red   = ((ush)bg_red   << 8);
                        green = ((ush)bg_green << 8);
                        blue  = ((ush)bg_blue  << 8);
 } else {
 /* this macro (from png.h) composites the foreground
                         * and background values and puts the result back into
                         * the first argument (== fg byte here:  safe) */
                        alpha_composite(r, r, a, bg_red);
                        alpha_composite(g, g, a, bg_green);
                        alpha_composite(b, b, a, bg_blue);
                        red   = ((ush)r << 8);
                        green = ((ush)g << 8);
                        blue  = ((ush)b << 8);
 }
                    pixel = ((red   >> RShift) & RMask) |
 ((green >> GShift) & GMask) |
 ((blue  >> BShift) & BMask);
 /* recall that we set ximage->byte_order = MSBFirst above */
 *dest++ = (char)((pixel >> 8) & 0xff);
 *dest++ = (char)( pixel        & 0xff);
 }
 }
 /* display after every 16 lines */
 if (((row+1) & 0xf) == 0) {
 XPutImage(display, window, gc, ximage, 0, (int)lastrow, 0,
 (int)lastrow, rpng2_info.width, 16);
 XFlush(display);
                lastrow = row + 1;
 }
 }

 } else /* depth == 8 */ {

 /* GRR:  add 8-bit support */

 }

 Trace((stderr, "calling final XPutImage()\n"))
 if (lastrow < startrow+height) {
 XPutImage(display, window, gc, ximage, 0, (int)lastrow, 0,
 (int)lastrow, rpng2_info.width, rpng2_info.height-lastrow);

         XFlush(display);
     }
 
 } /* end function rpng2_x_redisplay_image() */
