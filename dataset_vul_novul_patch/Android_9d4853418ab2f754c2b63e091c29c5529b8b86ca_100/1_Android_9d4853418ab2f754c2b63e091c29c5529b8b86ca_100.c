make_transform_images(png_store *ps)
 {
    png_byte colour_type = 0;
    png_byte bit_depth = 0;
    unsigned int palette_number = 0;
 
    /* This is in case of errors. */
   safecat(ps->test, sizeof ps->test, 0, "make standard images");
 
    /* Use next_format to enumerate all the combinations we test, including
    * generating multiple low bit depth palette images.
     */
   while (next_format(&colour_type, &bit_depth, &palette_number, 0))
    {
       int interlace_type;
 
 for (interlace_type = PNG_INTERLACE_NONE;
           interlace_type < INTERLACE_LAST; ++interlace_type)
 {

          char name[FILE_NAME_SIZE];
 
          standard_name(name, sizeof name, 0, colour_type, bit_depth,
            palette_number, interlace_type, 0, 0, 0);
         make_transform_image(ps, colour_type, bit_depth, palette_number,
             interlace_type, name);
       }
    }
 }
