test_size(png_modifier* PNG_CONST pm, png_byte PNG_CONST colour_type,
    int bdlo, int PNG_CONST bdhi)
 {
    /* Run the tests on each combination.
     *
    * NOTE: on my 32 bit x86 each of the following blocks takes
    * a total of 3.5 seconds if done across every combo of bit depth

     * width and height.  This is a waste of time in practice, hence the
     * hinc and winc stuff:
     */
   static PNG_CONST png_byte hinc[] = {1, 3, 11, 1, 5};
   static PNG_CONST png_byte winc[] = {1, 9, 5, 7, 1};
    for (; bdlo <= bdhi; ++bdlo)
    {
       png_uint_32 h, w;

 for (h=1; h<=16; h+=hinc[bdlo]) for (w=1; w<=16; w+=winc[bdlo])
 {
 /* First test all the 'size' images against the sequential
          * reader using libpng to deinterlace (where required.)  This
          * validates the write side of libpng.  There are four possibilities
          * to validate.
          */
         standard_test(&pm->this, FILEID(colour_type, DEPTH(bdlo), 0/*palette*/,
            PNG_INTERLACE_NONE, w, h, 0), 0/*do_interlace*/,
            pm->use_update_info);

 if (fail(pm))
 return 0;

         standard_test(&pm->this, FILEID(colour_type, DEPTH(bdlo), 0/*palette*/,
            PNG_INTERLACE_NONE, w, h, 1), 0/*do_interlace*/,
            pm->use_update_info);


          if (fail(pm))
             return 0;
 
#     ifdef PNG_WRITE_INTERLACING_SUPPORTED
         standard_test(&pm->this, FILEID(colour_type, DEPTH(bdlo), 0/*palette*/,
            PNG_INTERLACE_ADAM7, w, h, 0), 0/*do_interlace*/,
            pm->use_update_info);
         if (fail(pm))
            return 0;
         standard_test(&pm->this, FILEID(colour_type, DEPTH(bdlo), 0/*palette*/,
            PNG_INTERLACE_ADAM7, w, h, 1), 0/*do_interlace*/,
            pm->use_update_info);
         if (fail(pm))
            return 0;
#     endif
          /* Now validate the interlaced read side - do_interlace true,
           * in the progressive case this does actually make a difference
           * to the code used in the non-interlaced case too.
          */
         standard_test(&pm->this, FILEID(colour_type, DEPTH(bdlo), 0/*palette*/,
            PNG_INTERLACE_NONE, w, h, 0), 1/*do_interlace*/,
            pm->use_update_info);


          if (fail(pm))
             return 0;
 
 #     ifdef PNG_WRITE_INTERLACING_SUPPORTED
          standard_test(&pm->this, FILEID(colour_type, DEPTH(bdlo), 0/*palette*/,
             PNG_INTERLACE_ADAM7, w, h, 0), 1/*do_interlace*/,
             pm->use_update_info);


          if (fail(pm))
             return 0;
 #     endif
       }
    }
 
 return 1; /* keep going */
}
