make_errors(png_modifier* PNG_CONST pm, png_byte PNG_CONST colour_type,
make_errors(png_modifier* const pm, png_byte const colour_type,
    int bdlo, int const bdhi)
 {
    for (; bdlo <= bdhi; ++bdlo)
    {
 int interlace_type;

 for (interlace_type = PNG_INTERLACE_NONE;
           interlace_type < INTERLACE_LAST; ++interlace_type)
 {
 unsigned int test;

          char name[FILE_NAME_SIZE];
 
          standard_name(name, sizeof name, 0, colour_type, 1<<bdlo, 0,
            interlace_type, 0, 0, do_own_interlace);
 
         for (test=0; test<ARRAY_SIZE(error_test); ++test)
          {
             make_error(&pm->this, colour_type, DEPTH(bdlo), interlace_type,
                test, name);

 if (fail(pm))
 return 0;
 }
 }
 }

 return 1; /* keep going */
}
