png_set_filter(png_structp png_ptr, int method, int filters)
{
   png_debug(1, "in png_set_filter");

   if (png_ptr == NULL)
      return;
#ifdef PNG_MNG_FEATURES_SUPPORTED
   if ((png_ptr->mng_features_permitted & PNG_FLAG_MNG_FILTER_64) &&
      (method == PNG_INTRAPIXEL_DIFFERENCING))
         method = PNG_FILTER_TYPE_BASE;
#endif
   if (method == PNG_FILTER_TYPE_BASE)
    {
       switch (filters & (PNG_ALL_FILTERS | 0x07))
       {
         case PNG_FILTER_VALUE_NONE:
              png_ptr->do_filter = PNG_FILTER_NONE; break;
 #ifdef PNG_WRITE_FILTER_SUPPORTED
          case 5:
          case 6:
          case 7: png_warning(png_ptr, "Unknown row filter for method 0");
                 break;
          case PNG_FILTER_VALUE_SUB:
               png_ptr->do_filter = PNG_FILTER_SUB; break;
          case PNG_FILTER_VALUE_UP:
              png_ptr->do_filter = PNG_FILTER_UP; break;
         case PNG_FILTER_VALUE_AVG:
              png_ptr->do_filter = PNG_FILTER_AVG; break;
         case PNG_FILTER_VALUE_PAETH:
              png_ptr->do_filter = PNG_FILTER_PAETH; break;
          default: png_ptr->do_filter = (png_byte)filters; break;
 #else
          default: png_warning(png_ptr, "Unknown row filter for method 0");
                 break;
 #endif /* PNG_WRITE_FILTER_SUPPORTED */
       }
 
      /* If we have allocated the row_buf, this means we have already started
       * with the image and we should have allocated all of the filter buffers
       * that have been selected.  If prev_row isn't already allocated, then
       * it is too late to start using the filters that need it, since we
       * will be missing the data in the previous row.  If an application
       * wants to start and stop using particular filters during compression,
       * it should start out with all of the filters, and then add and
       * remove them after the start of compression.
       */
      if (png_ptr->row_buf != NULL)
      {
#ifdef PNG_WRITE_FILTER_SUPPORTED
         if ((png_ptr->do_filter & PNG_FILTER_SUB) && png_ptr->sub_row == NULL)
         {
            png_ptr->sub_row = (png_bytep)png_malloc(png_ptr,
              (png_ptr->rowbytes + 1));
            png_ptr->sub_row[0] = PNG_FILTER_VALUE_SUB;
         }

         if ((png_ptr->do_filter & PNG_FILTER_UP) && png_ptr->up_row == NULL)
         {
            if (png_ptr->prev_row == NULL)
            {
               png_warning(png_ptr, "Can't add Up filter after starting");
               png_ptr->do_filter &= ~PNG_FILTER_UP;
            }
            else
            {
               png_ptr->up_row = (png_bytep)png_malloc(png_ptr,
                  (png_ptr->rowbytes + 1));
               png_ptr->up_row[0] = PNG_FILTER_VALUE_UP;
            }
         }

         if ((png_ptr->do_filter & PNG_FILTER_AVG) && png_ptr->avg_row == NULL)
         {
            if (png_ptr->prev_row == NULL)
            {
               png_warning(png_ptr, "Can't add Average filter after starting");
               png_ptr->do_filter &= ~PNG_FILTER_AVG;
            }
            else
            {
               png_ptr->avg_row = (png_bytep)png_malloc(png_ptr,
                  (png_ptr->rowbytes + 1));
               png_ptr->avg_row[0] = PNG_FILTER_VALUE_AVG;
            }
         }

         if ((png_ptr->do_filter & PNG_FILTER_PAETH) &&
             png_ptr->paeth_row == NULL)
         {
            if (png_ptr->prev_row == NULL)
            {
               png_warning(png_ptr, "Can't add Paeth filter after starting");
               png_ptr->do_filter &= (png_byte)(~PNG_FILTER_PAETH);
            }
            else
            {
               png_ptr->paeth_row = (png_bytep)png_malloc(png_ptr,
                  (png_ptr->rowbytes + 1));
               png_ptr->paeth_row[0] = PNG_FILTER_VALUE_PAETH;
            }
         }

         if (png_ptr->do_filter == PNG_NO_FILTERS)
#endif /* PNG_WRITE_FILTER_SUPPORTED */
            png_ptr->do_filter = PNG_FILTER_NONE;
      }
   }
   else
      png_error(png_ptr, "Unknown custom filter method");
}
