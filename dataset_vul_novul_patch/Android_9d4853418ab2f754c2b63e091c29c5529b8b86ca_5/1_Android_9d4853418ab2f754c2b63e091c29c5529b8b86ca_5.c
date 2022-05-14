static void readpng2_warning_handler(png_structp png_ptr, png_const_charp msg)

 {
     fprintf(stderr, "readpng2 libpng warning: %s\n", msg);
     fflush(stderr);
 }
