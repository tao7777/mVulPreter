png_do_rgb_to_gray(png_structp png_ptr, png_row_infop row_info, png_bytep row)

{
   png_uint_32 i;

   png_uint_32 row_width = row_info->width;
   int rgb_error = 0;

   png_debug(1, "in png_do_rgb_to_gray");

   if (
#ifdef PNG_USELESS_TESTS_SUPPORTED
       row != NULL && row_info != NULL &&
#endif
      (row_info->color_type & PNG_COLOR_MASK_COLOR))
   {
      png_uint_32 rc = png_ptr->rgb_to_gray_red_coeff;
      png_uint_32 gc = png_ptr->rgb_to_gray_green_coeff;
      png_uint_32 bc = png_ptr->rgb_to_gray_blue_coeff;

      if (row_info->color_type == PNG_COLOR_TYPE_RGB)
      {
         if (row_info->bit_depth == 8)
         {
#if defined(PNG_READ_GAMMA_SUPPORTED) || defined(PNG_READ_BACKGROUND_SUPPORTED)
            if (png_ptr->gamma_from_1 != NULL && png_ptr->gamma_to_1 != NULL)
            {
               png_bytep sp = row;
               png_bytep dp = row;

               for (i = 0; i < row_width; i++)
               {
                  png_byte red   = png_ptr->gamma_to_1[*(sp++)];
                  png_byte green = png_ptr->gamma_to_1[*(sp++)];
                  png_byte blue  = png_ptr->gamma_to_1[*(sp++)];
                  if (red != green || red != blue)
                  {
                     rgb_error |= 1;
                     *(dp++) = png_ptr->gamma_from_1[
                       (rc*red + gc*green + bc*blue)>>15];
                  }
                  else
                     *(dp++) = *(sp - 1);
               }
            }
            else
#endif
            {
               png_bytep sp = row;
               png_bytep dp = row;
               for (i = 0; i < row_width; i++)
               {
                  png_byte red   = *(sp++);
                  png_byte green = *(sp++);
                  png_byte blue  = *(sp++);
                  if (red != green || red != blue)
                  {
                     rgb_error |= 1;
                     *(dp++) = (png_byte)((rc*red + gc*green + bc*blue)>>15);
                  }
                  else
                     *(dp++) = *(sp - 1);
               }
            }
         }

         else /* RGB bit_depth == 16 */
         {
#if defined(PNG_READ_GAMMA_SUPPORTED) || defined(PNG_READ_BACKGROUND_SUPPORTED)
            if (png_ptr->gamma_16_to_1 != NULL &&
                png_ptr->gamma_16_from_1 != NULL)
            {
               png_bytep sp = row;
               png_bytep dp = row;
                for (i = 0; i < row_width; i++)
                {
                   png_uint_16 red, green, blue, w;
                  png_byte hi,lo;
 
                  hi=*(sp)++; lo=*(sp)++;
                  red   = (png_uint_16)((hi << 8) | (lo));
                  hi=*(sp)++; lo=*(sp)++;
                  green = (png_uint_16)((hi << 8) | (lo));
                  hi=*(sp)++; lo=*(sp)++;
                  blue  = (png_uint_16)((hi << 8) | (lo));
 
                   if (red == green && red == blue)
                      w = red;
                  else
                  {
                     png_uint_16 red_1   = png_ptr->gamma_16_to_1[(red&0xff) >>
                                  png_ptr->gamma_shift][red>>8];
                     png_uint_16 green_1 =
                         png_ptr->gamma_16_to_1[(green&0xff) >>
                                  png_ptr->gamma_shift][green>>8];
                     png_uint_16 blue_1  = png_ptr->gamma_16_to_1[(blue&0xff) >>
                                  png_ptr->gamma_shift][blue>>8];
                     png_uint_16 gray16  = (png_uint_16)((rc*red_1 + gc*green_1
                                  + bc*blue_1)>>15);
                     w = png_ptr->gamma_16_from_1[(gray16&0xff) >>
                         png_ptr->gamma_shift][gray16 >> 8];
                     rgb_error |= 1;
                  }

                  *(dp++) = (png_byte)((w>>8) & 0xff);
                  *(dp++) = (png_byte)(w & 0xff);
               }
            }
            else
#endif
            {
               png_bytep sp = row;
               png_bytep dp = row;
                for (i = 0; i < row_width; i++)
                {
                   png_uint_16 red, green, blue, gray16;
                  png_byte hi,lo;
 
                  hi=*(sp)++; lo=*(sp)++;
                  red   = (png_uint_16)((hi << 8) | (lo));
                  hi=*(sp)++; lo=*(sp)++;
                  green = (png_uint_16)((hi << 8) | (lo));
                  hi=*(sp)++; lo=*(sp)++;
                  blue  = (png_uint_16)((hi << 8) | (lo));
 
                   if (red != green || red != blue)
                      rgb_error |= 1;
                  gray16  = (png_uint_16)((rc*red + gc*green + bc*blue)>>15);
                  *(dp++) = (png_byte)((gray16>>8) & 0xff);
                  *(dp++) = (png_byte)(gray16 & 0xff);
               }
            }
         }
      }
      if (row_info->color_type == PNG_COLOR_TYPE_RGB_ALPHA)
      {
         if (row_info->bit_depth == 8)
         {
#if defined(PNG_READ_GAMMA_SUPPORTED) || defined(PNG_READ_BACKGROUND_SUPPORTED)
            if (png_ptr->gamma_from_1 != NULL && png_ptr->gamma_to_1 != NULL)
            {
               png_bytep sp = row;
               png_bytep dp = row;
               for (i = 0; i < row_width; i++)
               {
                  png_byte red   = png_ptr->gamma_to_1[*(sp++)];
                  png_byte green = png_ptr->gamma_to_1[*(sp++)];
                  png_byte blue  = png_ptr->gamma_to_1[*(sp++)];
                  if (red != green || red != blue)
                     rgb_error |= 1;
                  *(dp++) =  png_ptr->gamma_from_1
                             [(rc*red + gc*green + bc*blue)>>15];
                  *(dp++) = *(sp++);  /* alpha */
               }
            }
            else
#endif
            {
               png_bytep sp = row;
               png_bytep dp = row;
               for (i = 0; i < row_width; i++)
               {
                  png_byte red   = *(sp++);
                  png_byte green = *(sp++);
                  png_byte blue  = *(sp++);
                  if (red != green || red != blue)
                     rgb_error |= 1;
                  *(dp++) =  (png_byte)((rc*red + gc*green + bc*blue)>>15);
                  *(dp++) = *(sp++);  /* alpha */
               }
            }
         }
         else /* RGBA bit_depth == 16 */
         {
#if defined(PNG_READ_GAMMA_SUPPORTED) || defined(PNG_READ_BACKGROUND_SUPPORTED)
            if (png_ptr->gamma_16_to_1 != NULL &&
                png_ptr->gamma_16_from_1 != NULL)
            {
               png_bytep sp = row;
               png_bytep dp = row;
                for (i = 0; i < row_width; i++)
                {
                   png_uint_16 red, green, blue, w;
                  png_byte hi,lo;

                  hi=*(sp)++; lo=*(sp)++;
                  red   = (png_uint_16)((hi << 8) | (lo));
                  hi=*(sp)++; lo=*(sp)++;
                  green = (png_uint_16)((hi << 8) | (lo));
                  hi=*(sp)++; lo=*(sp)++;
                  blue  = (png_uint_16)((hi << 8) | (lo));
 
                   if (red == green && red == blue)
                      w = red;
                  else
                  {
                     png_uint_16 red_1   = png_ptr->gamma_16_to_1[(red&0xff) >>
                         png_ptr->gamma_shift][red>>8];
                     png_uint_16 green_1 =
                         png_ptr->gamma_16_to_1[(green&0xff) >>
                         png_ptr->gamma_shift][green>>8];
                     png_uint_16 blue_1  = png_ptr->gamma_16_to_1[(blue&0xff) >>
                         png_ptr->gamma_shift][blue>>8];
                     png_uint_16 gray16  = (png_uint_16)((rc * red_1
                         + gc * green_1 + bc * blue_1)>>15);
                     w = png_ptr->gamma_16_from_1[(gray16&0xff) >>
                         png_ptr->gamma_shift][gray16 >> 8];
                     rgb_error |= 1;
                  }

                  *(dp++) = (png_byte)((w>>8) & 0xff);
                  *(dp++) = (png_byte)(w & 0xff);
                  *(dp++) = *(sp++);  /* alpha */
                  *(dp++) = *(sp++);
               }
            }
            else
#endif
            {
               png_bytep sp = row;
               png_bytep dp = row;
               for (i = 0; i < row_width; i++)
               {
                  png_uint_16 red, green, blue, gray16;
                  red   = (png_uint_16)((*(sp)<<8) | *(sp+1)); sp+=2;
                  green = (png_uint_16)((*(sp)<<8) | *(sp+1)); sp+=2;
                  blue  = (png_uint_16)((*(sp)<<8) | *(sp+1)); sp+=2;
                  if (red != green || red != blue)
                     rgb_error |= 1;
                  gray16  = (png_uint_16)((rc*red + gc*green + bc*blue)>>15);
                  *(dp++) = (png_byte)((gray16>>8) & 0xff);
                  *(dp++) = (png_byte)(gray16 & 0xff);
                  *(dp++) = *(sp++);  /* alpha */
                  *(dp++) = *(sp++);
               }
            }
         }
      }
   row_info->channels -= (png_byte)2;
      row_info->color_type &= ~PNG_COLOR_MASK_COLOR;
      row_info->pixel_depth = (png_byte)(row_info->channels *
         row_info->bit_depth);
      row_info->rowbytes = PNG_ROWBYTES(row_info->pixel_depth, row_width);
   }
   return rgb_error;
}
