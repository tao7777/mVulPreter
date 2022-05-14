read_png(struct control *control)
 /* Read a PNG, return 0 on success else an error (status) code; a bit mask as
    * defined for file::status_code as above.
    */

 {
    png_structp png_ptr;
    png_infop info_ptr = NULL;
   volatile png_bytep row = NULL, display = NULL;
    volatile int rc;
 
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, control,
      error_handler, warning_handler);

 if (png_ptr == NULL)
 {
 /* This is not really expected. */
      log_error(&control->file, LIBPNG_ERROR_CODE, "OOM allocating png_struct");
      control->file.status_code |= INTERNAL_ERROR;
 return LIBPNG_ERROR_CODE;
 }


    rc = setjmp(control->file.jmpbuf);
    if (rc == 0)
    {
       png_set_read_fn(png_ptr, control, read_callback);
 
       info_ptr = png_create_info_struct(png_ptr);
 if (info_ptr == NULL)
         png_error(png_ptr, "OOM allocating info structure");

 if (control->file.global->verbose)
         fprintf(stderr, " INFO\n");


       png_read_info(png_ptr, info_ptr);
 
       {
         png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
 
         row = png_voidcast(png_byte*, malloc(rowbytes));
         display = png_voidcast(png_byte*, malloc(rowbytes));
 
         if (row == NULL || display == NULL)
            png_error(png_ptr, "OOM allocating row buffers");
 
         {
            png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
            int passes = png_set_interlace_handling(png_ptr);
            int pass;
            png_start_read_image(png_ptr);
            for (pass = 0; pass < passes; ++pass)
            {
               png_uint_32 y = height;
               /* NOTE: this trashes the row each time; interlace handling won't
                * work, but this avoids memory thrashing for speed testing.
                */
               while (y-- > 0)
                  png_read_row(png_ptr, row, display);
            }
         }
       }
 
       if (control->file.global->verbose)
         fprintf(stderr, " END\n");

 /* Make sure to read to the end of the file: */
      png_read_end(png_ptr, info_ptr);

    }
 
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
   if (row != NULL) free(row);
   if (display != NULL) free(display);
    return rc;
 }
