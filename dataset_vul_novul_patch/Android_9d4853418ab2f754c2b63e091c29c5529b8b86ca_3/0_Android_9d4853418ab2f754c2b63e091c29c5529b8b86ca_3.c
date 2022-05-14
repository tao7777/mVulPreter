static void readpng2_info_callback(png_structp png_ptr, png_infop info_ptr)
{
    mainprog_info  *mainprog_ptr;
 int  color_type, bit_depth;
    png_uint_32 width, height;
#ifdef PNG_FLOATING_POINT_SUPPORTED
 double  gamma;
#else
    png_fixed_point gamma;
#endif


 /* setjmp() doesn't make sense here, because we'd either have to exit(),
     * longjmp() ourselves, or return control to libpng, which doesn't want
     * to see us again.  By not doing anything here, libpng will instead jump
     * to readpng2_decode_data(), which can return an error value to the main
     * program. */


 /* retrieve the pointer to our special-purpose struct, using the png_ptr
     * that libpng passed back to us (i.e., not a global this time--there's
     * no real difference for a single image, but for a multithreaded browser
     * decoding several PNG images at the same time, one needs to avoid mixing
     * up different images' structs) */

    mainprog_ptr = png_get_progressive_ptr(png_ptr);

 if (mainprog_ptr == NULL) { /* we be hosed */
        fprintf(stderr,
 "readpng2 error:  main struct not recoverable in info_callback.\n");
        fflush(stderr);
 return;
 /*
         * Alternatively, we could call our error-handler just like libpng
         * does, which would effectively terminate the program.  Since this
         * can only happen if png_ptr gets redirected somewhere odd or the
         * main PNG struct gets wiped, we're probably toast anyway.  (If
         * png_ptr itself is NULL, we would not have been called.)
         */
 }


 /* this is just like in the non-progressive case */

    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
       NULL, NULL, NULL);
    mainprog_ptr->width = (ulg)width;
    mainprog_ptr->height = (ulg)height;



     /* since we know we've read all of the PNG file's "header" (i.e., up
      * to IDAT), we can check for a background color here */
 
    if (mainprog_ptr->need_bgcolor)
     {
         png_color_16p pBackground;
 
         /* it is not obvious from the libpng documentation, but this function
          * takes a pointer to a pointer, and it always returns valid red,
          * green and blue values, regardless of color_type: */
        if (png_get_bKGD(png_ptr, info_ptr, &pBackground))
        {
 
           /* however, it always returns the raw bKGD data, regardless of any
            * bit-depth transformations, so check depth and adjust if necessary
            */
           if (bit_depth == 16) {
               mainprog_ptr->bg_red   = pBackground->red   >> 8;
               mainprog_ptr->bg_green = pBackground->green >> 8;
               mainprog_ptr->bg_blue  = pBackground->blue  >> 8;
           } else if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
               if (bit_depth == 1)
                   mainprog_ptr->bg_red = mainprog_ptr->bg_green =
                     mainprog_ptr->bg_blue = pBackground->gray? 255 : 0;
               else if (bit_depth == 2)
                   mainprog_ptr->bg_red = mainprog_ptr->bg_green =
                     mainprog_ptr->bg_blue = (255/3) * pBackground->gray;
               else /* bit_depth == 4 */
                   mainprog_ptr->bg_red = mainprog_ptr->bg_green =
                     mainprog_ptr->bg_blue = (255/15) * pBackground->gray;
           } else {
               mainprog_ptr->bg_red   = (uch)pBackground->red;
               mainprog_ptr->bg_green = (uch)pBackground->green;
               mainprog_ptr->bg_blue  = (uch)pBackground->blue;
           }
         }
     }
 

 /* as before, let libpng expand palette images to RGB, low-bit-depth
     * grayscale images to 8 bits, transparency chunks to full alpha channel;
     * strip 16-bit-per-sample images to 8 bits per sample; and convert
     * grayscale to RGB[A] */

 if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_expand(png_ptr);
 if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand(png_ptr);
 if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_expand(png_ptr);
#ifdef PNG_READ_16_TO_8_SUPPORTED
 if (bit_depth == 16)
#  ifdef PNG_READ_SCALE_16_TO_8_SUPPORTED
        png_set_scale_16(png_ptr);
#  else
        png_set_strip_16(png_ptr);
#  endif
#endif
 if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ptr);


 /* Unlike the basic viewer, which was designed to operate on local files,
     * this program is intended to simulate a web browser--even though we
     * actually read from a local file, too.  But because we are pretending
     * that most of the images originate on the Internet, we follow the recom-
     * mendation of the sRGB proposal and treat unlabelled images (no gAMA
     * chunk) as existing in the sRGB color space.  That is, we assume that
     * such images have a file gamma of 0.45455, which corresponds to a PC-like
     * display system.  This change in assumptions will have no effect on a
     * PC-like system, but on a Mac, SGI, NeXT or other system with a non-
     * identity lookup table, it will darken unlabelled images, which effec-
     * tively favors images from PC-like systems over those originating on
     * the local platform.  Note that mainprog_ptr->display_exponent is the
     * "gamma" value for the entire display system, i.e., the product of
     * LUT_exponent and CRT_exponent. */

#ifdef PNG_FLOATING_POINT_SUPPORTED
 if (png_get_gAMA(png_ptr, info_ptr, &gamma))
        png_set_gamma(png_ptr, mainprog_ptr->display_exponent, gamma);
 else
        png_set_gamma(png_ptr, mainprog_ptr->display_exponent, 0.45455);
#else
 if (png_get_gAMA_fixed(png_ptr, info_ptr, &gamma))
        png_set_gamma_fixed(png_ptr,
 (png_fixed_point)(100000*mainprog_ptr->display_exponent+.5), gamma);
 else
        png_set_gamma_fixed(png_ptr,
 (png_fixed_point)(100000*mainprog_ptr->display_exponent+.5), 45455);
#endif

 /* we'll let libpng expand interlaced images, too */

    mainprog_ptr->passes = png_set_interlace_handling(png_ptr);


 /* all transformations have been registered; now update info_ptr data and
     * then get rowbytes and channels */

    png_read_update_info(png_ptr, info_ptr);

    mainprog_ptr->rowbytes = (int)png_get_rowbytes(png_ptr, info_ptr);
    mainprog_ptr->channels = png_get_channels(png_ptr, info_ptr);


 /* Call the main program to allocate memory for the image buffer and
     * initialize windows and whatnot.  (The old-style function-pointer
     * invocation is used for compatibility with a few supposedly ANSI
     * compilers that nevertheless barf on "fn_ptr()"-style syntax.) */

 (*mainprog_ptr->mainprog_init)();


 /* and that takes care of initialization */

 return;
}
