check_interlace_type(int PNG_CONST interlace_type)
 {
    if (interlace_type != PNG_INTERLACE_NONE)
    {
       /* This is an internal error - --interlace tests should be skipped, not
       * attempted.
       */
      fprintf(stderr, "pngvalid: no interlace support\n");

       exit(99);
    }
 }
