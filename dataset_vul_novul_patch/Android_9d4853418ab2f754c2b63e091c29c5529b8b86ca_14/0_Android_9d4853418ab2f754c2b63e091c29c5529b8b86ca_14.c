 generate_row(png_bytep row, size_t rowbytes, unsigned int y, int color_type,
    int bit_depth, png_const_bytep gamma_table, double conv,
   unsigned int *colors, int small)
 {
   int filters = 0; /* file *MASK*, 0 means the default, not NONE */
   png_uint_32 size_max =
      image_size_of_type(color_type, bit_depth, colors, small)-1;
    png_uint_32 depth_max = (1U << bit_depth)-1; /* up to 65536 */
 
   if (colors[0] == 0) if (small)
   {
      unsigned int pixel_depth = pixel_depth_of_type(color_type, bit_depth);

      /* For pixel depths less than 16 generate a single row containing all the
       * possible pixel values.  For 16 generate all 65536 byte pair
       * combinations in a 256x256 pixel array.
       */
      switch (pixel_depth)
      {
         case 1:
            assert(y == 0 && rowbytes == 1 && size_max == 1);
            row[0] = 0x6CU; /* binary: 01101100, only top 2 bits used */
            filters = PNG_FILTER_NONE;
            break;

         case 2:
            assert(y == 0 && rowbytes == 1 && size_max == 3);
            row[0] = 0x1BU; /* binary 00011011, all bits used */
            filters = PNG_FILTER_NONE;
            break;

         case 4:
            assert(y == 0 && rowbytes == 8 && size_max == 15);
            row[0] = 0x01U;
            row[1] = 0x23U; /* SUB gives 0x22U for all following bytes */
            row[2] = 0x45U;
            row[3] = 0x67U;
            row[4] = 0x89U;
            row[5] = 0xABU;
            row[6] = 0xCDU;
            row[7] = 0xEFU;
            filters = PNG_FILTER_SUB;
            break;

         case 8:
            /* The row will have all the pixel values in order starting with
             * '1', the SUB filter will change every byte into '1' (including
             * the last, which generates pixel value '0').  Since the SUB filter
             * has value 1 this should result in maximum compression.
             */
            assert(y == 0 && rowbytes == 256 && size_max == 255);
            for (;;)
            {
               row[size_max] = 0xFFU & (size_max+1);
               if (size_max == 0)
                  break;
               --size_max;
            }
            filters = PNG_FILTER_SUB;
            break;

         case 16:
            /* Rows are generated such that each row has a constant difference
             * between the first and second byte of each pixel and so that the
             * difference increases by 1 at each row.  The rows start with the
             * first byte value of 0 and the value increases to 255 across the
             * row.
             *
             * The difference starts at 1, so the first row is:
             *
             *     0 1 1 2 2 3 3 4 ... 254 255 255 0
             *
             * This means that running the SUB filter on the first row produces:
             *
             *   [SUB==1] 0 1 0 1 0 1...
             *
             * Then the difference is 2 on the next row, giving:
             *
             *    0 2 1 3 2 4 3 5 ... 254 0 255 1
             *
             * When the UP filter is run on this libpng produces:
             *
             *   [UP ==2] 0 1 0 1 0 1...
             *
             * And so on for all the remain rows to the final two * rows:
             *
             *    row 254: 0 255 1 0 2 1 3 2 4 3 ... 254 253 255 254
             *    row 255: 0   0 1 1 2 2 3 3 4 4 ... 254 254 255 255
             */
            assert(rowbytes == 512 && size_max == 255);
            for (;;)
            {
               row[2*size_max  ] = 0xFFU & size_max;
               row[2*size_max+1] = 0xFFU & (size_max+y+1);
               if (size_max == 0)
                  break;
               --size_max;
            }
            /* The first row must include PNG_FILTER_UP so that libpng knows we
             * need to keep it for the following row:
             */
            filters = (y == 0 ? PNG_FILTER_SUB+PNG_FILTER_UP : PNG_FILTER_UP);
            break;

         case 24:
         case 32:
         case 48:
         case 64:
            /* The rows are filled by an alogorithm similar to the above, in the
             * first row pixel bytes are all equal, increasing from 0 by 1 for
             * each pixel.  In the second row the bytes within a pixel are
             * incremented 1,3,5,7,... from the previous row byte.  Using an odd
             * number ensures all the possible byte values are used.
             */
            assert(size_max == 255 && rowbytes == 256*(pixel_depth>>3));
            pixel_depth >>= 3; /* now in bytes */
            while (rowbytes > 0)
            {
               const size_t pixel_index = --rowbytes/pixel_depth;

               if (y == 0)
                  row[rowbytes] = 0xFFU & pixel_index;

               else
               {
                  const size_t byte_offset =
                     rowbytes - pixel_index * pixel_depth;

                  row[rowbytes] =
                     0xFFU & (pixel_index + (byte_offset * 2*y) + 1);
               }
            }
            filters = (y == 0 ? PNG_FILTER_SUB+PNG_FILTER_UP : PNG_FILTER_UP);
            break;

         default:
            assert(0/*NOT REACHED*/);
      }
   }

   else switch (channels_of_type(color_type))
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

   return filters;
 }
