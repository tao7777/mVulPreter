int readpng2_init(mainprog_info *mainprog_ptr)
{
    png_structp  png_ptr; /* note:  temporary variables! */
    png_infop  info_ptr;


 
     /* could also replace libpng warning-handler (final NULL), but no need: */
 
    png_ptr = png_create_read_struct(png_get_libpng_ver(NULL), mainprog_ptr,
       readpng2_error_handler, readpng2_warning_handler);
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
     * libpng function, unless an alternate error handler was installed--
     * but compatible error handlers must either use longjmp() themselves
     * (as in this program) or exit immediately, so here we are: */

 if (setjmp(mainprog_ptr->jmpbuf)) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
 return 2;
 }


#ifdef PNG_HANDLE_AS_UNKNOWN_SUPPORTED
 /* prepare the reader to ignore all recognized chunks whose data won't be
     * used, i.e., all chunks recognized by libpng except for IHDR, PLTE, IDAT,
     * IEND, tRNS, bKGD, gAMA, and sRGB (small performance improvement) */
 {
 /* These byte strings were copied from png.h.  If a future version
         * of readpng2.c recognizes more chunks, add them to this list.
         */
 static PNG_CONST png_byte chunks_to_process[] = {
 98, 75, 71, 68, '\0', /* bKGD */
 103, 65, 77, 65, '\0', /* gAMA */
 115, 82, 71, 66, '\0', /* sRGB */
 };

 /* Ignore all chunks except for IHDR, PLTE, tRNS, IDAT, and IEND */
       png_set_keep_unknown_chunks(png_ptr, -1 /* PNG_HANDLE_CHUNK_NEVER */,
          NULL, -1);

 /* But do not ignore chunks in the "chunks_to_process" list */
       png_set_keep_unknown_chunks(png_ptr,
 0 /* PNG_HANDLE_CHUNK_AS_DEFAULT */, chunks_to_process,
 sizeof(chunks_to_process)/5);
 }
#endif /* PNG_HANDLE_AS_UNKNOWN_SUPPORTED */


 /* instead of doing png_init_io() here, now we set up our callback
     * functions for progressive decoding */

    png_set_progressive_read_fn(png_ptr, mainprog_ptr,
      readpng2_info_callback, readpng2_row_callback, readpng2_end_callback);


 /* make sure we save our pointers for use in readpng2_decode_data() */

    mainprog_ptr->png_ptr = png_ptr;
    mainprog_ptr->info_ptr = info_ptr;


 /* and that's all there is to initialization */

 return 0;
}
