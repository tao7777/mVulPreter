test_standard(png_modifier* PNG_CONST pm, png_byte PNG_CONST colour_type,
    int bdlo, int PNG_CONST bdhi)
 {
    for (; bdlo <= bdhi; ++bdlo)
    {
 int interlace_type;

 for (interlace_type = PNG_INTERLACE_NONE;

            interlace_type < INTERLACE_LAST; ++interlace_type)
       {
          standard_test(&pm->this, FILEID(colour_type, DEPTH(bdlo), 0/*palette*/,
            interlace_type, 0, 0, 0), 0/*do_interlace*/, pm->use_update_info);
 
          if (fail(pm))
             return 0;
 }
 }

 return 1; /* keep going */
}
