png_do_write_interlace(png_row_infop row_info, png_bytep row, int pass)
 {
    /* Arrays to facilitate easy interlacing - use pass (0 - 6) as index */
 
#ifndef PNG_USE_GLOBAL_ARRAYS
    /* Start of interlace block */
    int png_pass_start[7] = {0, 4, 0, 2, 0, 1, 0};
 
    /* Offset to next interlace block */
    int png_pass_inc[7] = {8, 8, 4, 4, 2, 2, 1};
#endif
 
    png_debug(1, "in png_do_write_interlace");
 
   /* We don't have to do anything on the last pass (6) */
#ifdef PNG_USELESS_TESTS_SUPPORTED
   if (row != NULL && row_info != NULL && pass < 6)
#else
   if (pass < 6)
#endif
   {
      /* Each pixel depth is handled separately */
      switch (row_info->pixel_depth)
      {
         case 1:
         {
            png_bytep sp;
            png_bytep dp;
            int shift;
            int d;
            int value;
            png_uint_32 i;
            png_uint_32 row_width = row_info->width;

            dp = row;
            d = 0;
            shift = 7;
            for (i = png_pass_start[pass]; i < row_width;
               i += png_pass_inc[pass])
            {
               sp = row + (png_size_t)(i >> 3);
               value = (int)(*sp >> (7 - (int)(i & 0x07))) & 0x01;
               d |= (value << shift);

               if (shift == 0)
               {
                  shift = 7;
                  *dp++ = (png_byte)d;
                  d = 0;
               }
               else
                  shift--;

            }
            if (shift != 7)
               *dp = (png_byte)d;
            break;
         }
         case 2:
         {
            png_bytep sp;
            png_bytep dp;
            int shift;
            int d;
            int value;
            png_uint_32 i;
            png_uint_32 row_width = row_info->width;

            dp = row;
            shift = 6;
            d = 0;
            for (i = png_pass_start[pass]; i < row_width;
               i += png_pass_inc[pass])
            {
               sp = row + (png_size_t)(i >> 2);
               value = (*sp >> ((3 - (int)(i & 0x03)) << 1)) & 0x03;
               d |= (value << shift);

               if (shift == 0)
               {
                  shift = 6;
                  *dp++ = (png_byte)d;
                  d = 0;
               }
               else
                  shift -= 2;
            }
            if (shift != 6)
                   *dp = (png_byte)d;
            break;
         }
         case 4:
         {
            png_bytep sp;
            png_bytep dp;
            int shift;
            int d;
            int value;
            png_uint_32 i;
            png_uint_32 row_width = row_info->width;

            dp = row;
            shift = 4;
            d = 0;
            for (i = png_pass_start[pass]; i < row_width;
               i += png_pass_inc[pass])
            {
               sp = row + (png_size_t)(i >> 1);
               value = (*sp >> ((1 - (int)(i & 0x01)) << 2)) & 0x0f;
               d |= (value << shift);

               if (shift == 0)
               {
                  shift = 4;
                  *dp++ = (png_byte)d;
                  d = 0;
               }
               else
                  shift -= 4;
            }
            if (shift != 4)
               *dp = (png_byte)d;
            break;
         }
         default:
         {
            png_bytep sp;
            png_bytep dp;
            png_uint_32 i;
            png_uint_32 row_width = row_info->width;
            png_size_t pixel_bytes;

            /* Start at the beginning */
            dp = row;
            /* Find out how many bytes each pixel takes up */
            pixel_bytes = (row_info->pixel_depth >> 3);
            /* Loop through the row, only looking at the pixels that
               matter */
            for (i = png_pass_start[pass]; i < row_width;
               i += png_pass_inc[pass])
            {
               /* Find out where the original pixel is */
               sp = row + (png_size_t)i * pixel_bytes;
               /* Move the pixel */
               if (dp != sp)
                  png_memcpy(dp, sp, pixel_bytes);
               /* Next pixel */
               dp += pixel_bytes;
            }
            break;
         }
      }
      /* Set new row width */
      row_info->width = (row_info->width +
         png_pass_inc[pass] - 1 -
         png_pass_start[pass]) /
         png_pass_inc[pass];
         row_info->rowbytes = PNG_ROWBYTES(row_info->pixel_depth,
            row_info->width);
   }
}
