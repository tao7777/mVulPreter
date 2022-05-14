check_interlace_type(int PNG_CONST interlace_type)
check_interlace_type(int const interlace_type)
 {
   /* Prior to 1.7.0 libpng does not support the write of an interlaced image
    * unless PNG_WRITE_INTERLACING_SUPPORTED, even with do_interlace so the
    * code here does the pixel interlace itself, so:
    */
    if (interlace_type != PNG_INTERLACE_NONE)
    {
       /* This is an internal error - --interlace tests should be skipped, not
       * attempted.
       */
      fprintf(stderr, "pngvalid: no interlace support\n");

       exit(99);
    }
 }
