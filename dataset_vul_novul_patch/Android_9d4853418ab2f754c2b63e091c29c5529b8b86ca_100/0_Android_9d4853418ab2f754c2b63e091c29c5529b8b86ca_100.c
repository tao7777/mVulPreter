make_transform_images(png_store *ps)
make_transform_images(png_modifier *pm)
 {
    png_byte colour_type = 0;
    png_byte bit_depth = 0;
    unsigned int palette_number = 0;
 
    /* This is in case of errors. */
   safecat(pm->this.test, sizeof pm->this.test, 0, "make standard images");
 
    /* Use next_format to enumerate all the combinations we test, including
    * generating multiple low bit depth palette images. Non-A images (palette
    * and direct) are created with and without tRNS chunks.
     */
   while (next_format(&colour_type, &bit_depth, &palette_number, 1, 1))
    {
       int interlace_type;
 
 for (interlace_type = PNG_INTERLACE_NONE;
           interlace_type < INTERLACE_LAST; ++interlace_type)
 {

          char name[FILE_NAME_SIZE];
 
          standard_name(name, sizeof name, 0, colour_type, bit_depth,
            palette_number, interlace_type, 0, 0, do_own_interlace);
         make_transform_image(&pm->this, colour_type, bit_depth, palette_number,
             interlace_type, name);
       }
    }
 }
