 perform_transform_test(png_modifier *pm)
 {
   png_byte colour_type = 0;

    png_byte bit_depth = 0;
    unsigned int palette_number = 0;
 
   while (next_format(&colour_type, &bit_depth, &palette_number, pm->test_lbg,
            pm->test_tRNS))
    {
       png_uint_32 counter = 0;
       size_t base_pos;
 char name[64];

      base_pos = safecat(name, sizeof name, 0, "transform:");


       for (;;)
       {
          size_t pos = base_pos;
         const image_transform *list = 0;
 
          /* 'max' is currently hardwired to '1'; this should be settable on the
           * command line.
          */
         counter = image_transform_add(&list, 1/*max*/, counter,
            name, sizeof name, &pos, colour_type, bit_depth);

 if (counter == 0)
 break;

 /* The command line can change this to checking interlaced images. */
 do
 {
            pm->repeat = 0;
            transform_test(pm, FILEID(colour_type, bit_depth, palette_number,
               pm->interlace_type, 0, 0, 0), list, name);

 if (fail(pm))
 return;
 }
 while (pm->repeat);
 }
 }
}
