int readpng_init(FILE *infile, ulg *pWidth, ulg *pHeight)
{
    uch sig[8];


 /* first do a quick check that the file really is a PNG image; could
     * have used slightly more general png_sig_cmp() function instead */

    fread(sig, 1, 8, infile);
 if (png_sig_cmp(sig, 0, 8))
 return 1; /* bad signature */


 
     /* could pass pointers to user-defined error handlers instead of NULLs: */
 
    png_ptr = png_create_read_struct(png_get_libpng_ver(NULL), NULL, NULL,
        NULL);
     if (!png_ptr)
         return 4;   /* out of memory */
 
    info_ptr = png_create_info_struct(png_ptr);
 if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
 return 4; /* out of memory */
 }


 /* we could create a second info struct here (end_info), but it's only
     * useful if we want to keep pre- and post-IDAT chunk info separated
     * (mainly for PNG-aware image editors and converters) */


 /* setjmp() must be called in every function that calls a PNG-reading
     * libpng function */

 if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
 return 2;
 }


    png_init_io(png_ptr, infile);
    png_set_sig_bytes(png_ptr, 8); /* we already read the 8 signature bytes */

    png_read_info(png_ptr, info_ptr); /* read all PNG info up to image data */


 /* alternatively, could make separate calls to png_get_image_width(),
     * etc., but want bit_depth and color_type for later [don't care about
     * compression_type and filter_type => NULLs] */

    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
      NULL, NULL, NULL);
 *pWidth = width;
 *pHeight = height;


 /* OK, that's all we need for now; return happy */

 return 0;
}
