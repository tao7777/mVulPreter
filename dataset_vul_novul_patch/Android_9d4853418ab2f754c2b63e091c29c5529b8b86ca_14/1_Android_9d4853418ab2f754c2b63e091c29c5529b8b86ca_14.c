 generate_row(png_bytep row, size_t rowbytes, unsigned int y, int color_type,
    int bit_depth, png_const_bytep gamma_table, double conv,
   unsigned int *colors)
 {
   png_uint_32 size_max = image_size_of_type(color_type, bit_depth, colors)-1;
    png_uint_32 depth_max = (1U << bit_depth)-1; /* up to 65536 */
 
   if (colors[0] == 0) switch (channels_of_type(color_type))
    {
    /* 1 channel: a square image with a diamond, the least luminous colors are on
     *    the edge of the image, the most luminous in the center.
    */
 case 1:
 {
            png_uint_32 x;
            png_uint_32 base = 2*size_max - abs(2*y-size_max);

 for (x=0; x<=size_max; ++x)
 {
               png_uint_32 luma = base - abs(2*x-size_max);

 /* 'luma' is now in the range 0..2*size_max, we need
                * 0..depth_max
                */
               luma = (luma*depth_max + size_max) / (2*size_max);
               set_value(row, rowbytes, x, bit_depth, luma, gamma_table, conv);
 }
 }
 break;

 /* 2 channels: the color channel increases in luminosity from top to bottom,
    *    the alpha channel increases in opacity from left to right.
    */
 case 2:
 {
            png_uint_32 alpha = (depth_max * y * 2 + size_max) / (2 * size_max);
            png_uint_32 x;

 for (x=0; x<=size_max; ++x)
 {
               set_value(row, rowbytes, 2*x, bit_depth,
 (depth_max * x * 2 + size_max) / (2 * size_max), gamma_table,
                  conv);
               set_value(row, rowbytes, 2*x+1, bit_depth, alpha, gamma_table,
                  conv);
 }
 }
 break;

 /* 3 channels: linear combinations of, from the top-left corner clockwise,
    *    black, green, white, red.
    */
 case 3:
 {
 /* x0: the black->red scale (the value of the red component) at the
             *     start of the row (blue and green are 0).
             * x1: the green->white scale (the value of the red and blue
             *     components at the end of the row; green is depth_max).
             */
            png_uint_32 Y = (depth_max * y * 2 + size_max) / (2 * size_max);
            png_uint_32 x;

 /* Interpolate x/depth_max from start to end:
             *
             *        start end         difference
             * red:     Y    Y            0
             * green:   0   depth_max   depth_max
             * blue:    0    Y            Y
             */
 for (x=0; x<=size_max; ++x)
 {
               set_value(row, rowbytes, 3*x+0, bit_depth, /* red */ Y,
                     gamma_table, conv);
               set_value(row, rowbytes, 3*x+1, bit_depth, /* green */
 (depth_max * x * 2 + size_max) / (2 * size_max),
                  gamma_table, conv);
               set_value(row, rowbytes, 3*x+2, bit_depth, /* blue */
 (Y * x * 2 + size_max) / (2 * size_max),
                  gamma_table, conv);
 }
 }
 break;

 /* 4 channels: linear combinations of, from the top-left corner clockwise,
    *    transparent, red, green, blue.
    */
 case 4:
 {
 /* x0: the transparent->blue scale (the value of the blue and alpha
             *     components) at the start of the row (red and green are 0).
             * x1: the red->green scale (the value of the red and green
             *     components at the end of the row; blue is 0 and alpha is
             *     depth_max).
             */
            png_uint_32 Y = (depth_max * y * 2 + size_max) / (2 * size_max);
            png_uint_32 x;

 /* Interpolate x/depth_max from start to end:
             *
             *        start    end       difference
             * red:     0   depth_max-Y depth_max-Y
             * green:   0       Y             Y
             * blue:    Y       0            -Y
             * alpha:   Y    depth_max  depth_max-Y
             */
 for (x=0; x<=size_max; ++x)
 {
               set_value(row, rowbytes, 4*x+0, bit_depth, /* red */
 ((depth_max-Y) * x * 2 + size_max) / (2 * size_max),
                  gamma_table, conv);
               set_value(row, rowbytes, 4*x+1, bit_depth, /* green */
 (Y * x * 2 + size_max) / (2 * size_max),
                  gamma_table, conv);
               set_value(row, rowbytes, 4*x+2, bit_depth, /* blue */
                  Y - (Y * x * 2 + size_max) / (2 * size_max),
                  gamma_table, conv);
               set_value(row, rowbytes, 4*x+3, bit_depth, /* alpha */
                  Y + ((depth_max-Y) * x * 2 + size_max) / (2 * size_max),
                  gamma_table, conv);
 }
 }
 break;

 default:
         fprintf(stderr, "makepng: internal bad channel count\n");
         exit(2);
 }

 else if (color_type & PNG_COLOR_MASK_PALETTE)
 {
 /* Palette with fixed color: the image rows are all 0 and the image width
       * is 16.
       */
      memset(row, 0, rowbytes);
 }

 else if (colors[0] == channels_of_type(color_type))
 switch (channels_of_type(color_type))
 {
 case 1:
 {
 const png_uint_32 luma = colors[1];
               png_uint_32 x;

 for (x=0; x<=size_max; ++x)
                  set_value(row, rowbytes, x, bit_depth, luma, gamma_table,
                     conv);
 }
 break;

 case 2:
 {
 const png_uint_32 luma = colors[1];
 const png_uint_32 alpha = colors[2];
               png_uint_32 x;

 for (x=0; x<size_max; ++x)
 {
                  set_value(row, rowbytes, 2*x, bit_depth, luma, gamma_table,
                     conv);
                  set_value(row, rowbytes, 2*x+1, bit_depth, alpha, gamma_table,
                     conv);
 }
 }
 break;

 case 3:
 {
 const png_uint_32 red = colors[1];
 const png_uint_32 green = colors[2];
 const png_uint_32 blue = colors[3];
               png_uint_32 x;

 for (x=0; x<=size_max; ++x)
 {
                  set_value(row, rowbytes, 3*x+0, bit_depth, red, gamma_table,
                     conv);
                  set_value(row, rowbytes, 3*x+1, bit_depth, green, gamma_table,
                     conv);
                  set_value(row, rowbytes, 3*x+2, bit_depth, blue, gamma_table,
                     conv);
 }
 }
 break;

 case 4:
 {
 const png_uint_32 red = colors[1];
 const png_uint_32 green = colors[2];
 const png_uint_32 blue = colors[3];
 const png_uint_32 alpha = colors[4];
               png_uint_32 x;

 for (x=0; x<=size_max; ++x)
 {
                  set_value(row, rowbytes, 4*x+0, bit_depth, red, gamma_table,
                     conv);
                  set_value(row, rowbytes, 4*x+1, bit_depth, green, gamma_table,
                     conv);
                  set_value(row, rowbytes, 4*x+2, bit_depth, blue, gamma_table,
                     conv);
                  set_value(row, rowbytes, 4*x+3, bit_depth, alpha, gamma_table,
                     conv);
 }
 }
 break;

 default:
            fprintf(stderr, "makepng: internal bad channel count\n");
            exit(2);
 }

 else
 {
      fprintf(stderr,
 "makepng: --color: count(%u) does not match channels(%u)\n",

          colors[0], channels_of_type(color_type));
       exit(1);
    }
 }
