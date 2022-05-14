 next_format(png_bytep colour_type, png_bytep bit_depth,
   unsigned int* palette_number, int no_low_depth_gray)
 {
    if (*bit_depth == 0)
    {
       *colour_type = 0;
      if (no_low_depth_gray)
         *bit_depth = 8;
      else
          *bit_depth = 1;
       *palette_number = 0;
       return 1;
    }
 
   if (*colour_type == 3)
    {
      /* Add multiple palettes for colour type 3. */
      if (++*palette_number < PALETTE_COUNT(*bit_depth))
          return 1;
 
       *palette_number = 0;
    }
 
 *bit_depth = (png_byte)(*bit_depth << 1);

 
    /* Palette images are restricted to 8 bit depth */
    if (*bit_depth <= 8
#     ifdef DO_16BIT
          || (*colour_type != 3 && *bit_depth <= 16)
#     endif
       )
       return 1;
 
 /* Move to the next color type, or return 0 at the end. */
 switch (*colour_type)
 {
 case 0:
 *colour_type = 2;
 *bit_depth = 8;
 return 1;

 case 2:
 *colour_type = 3;
 *bit_depth = 1;
 return 1;

 case 3:
 *colour_type = 4;
 *bit_depth = 8;
 return 1;

 case 4:
 *colour_type = 6;
 *bit_depth = 8;
 return 1;

 default:
 return 0;
 }
}
