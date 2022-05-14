 png_write_start_row(png_structp png_ptr)
 {
 #ifdef PNG_WRITE_INTERLACING_SUPPORTED
#ifndef PNG_USE_GLOBAL_ARRAYS
    /* Arrays to facilitate easy interlacing - use pass (0 - 6) as index */
 
    /* Start of interlace block */
   int png_pass_start[7] = {0, 4, 0, 2, 0, 1, 0};

   /* Offset to next interlace block */
   int png_pass_inc[7] = {8, 8, 4, 4, 2, 2, 1};

   /* Start of interlace block in the y direction */
   int png_pass_ystart[7] = {0, 0, 4, 0, 2, 0, 1};
 
    /* Offset to next interlace block in the y direction */
    int png_pass_yinc[7] = {8, 8, 8, 4, 4, 2, 2};
#endif
 #endif
 
    png_size_t buf_size;

   png_debug(1, "in png_write_start_row");

   buf_size = (png_size_t)(PNG_ROWBYTES(
      png_ptr->usr_channels*png_ptr->usr_bit_depth, png_ptr->width) + 1);

   /* Set up row buffer */
   png_ptr->row_buf = (png_bytep)png_malloc(png_ptr,
     (png_uint_32)buf_size);
   png_ptr->row_buf[0] = PNG_FILTER_VALUE_NONE;

#ifdef PNG_WRITE_FILTER_SUPPORTED
   /* Set up filtering buffer, if using this filter */
   if (png_ptr->do_filter & PNG_FILTER_SUB)
   {
      png_ptr->sub_row = (png_bytep)png_malloc(png_ptr,
         (png_uint_32)(png_ptr->rowbytes + 1));
      png_ptr->sub_row[0] = PNG_FILTER_VALUE_SUB;
   }

   /* We only need to keep the previous row if we are using one of these. */
   if (png_ptr->do_filter & (PNG_FILTER_AVG | PNG_FILTER_UP | PNG_FILTER_PAETH))
   {
      /* Set up previous row buffer */
      png_ptr->prev_row = (png_bytep)png_calloc(png_ptr,
         (png_uint_32)buf_size);

      if (png_ptr->do_filter & PNG_FILTER_UP)
      {
         png_ptr->up_row = (png_bytep)png_malloc(png_ptr,
            (png_uint_32)(png_ptr->rowbytes + 1));
         png_ptr->up_row[0] = PNG_FILTER_VALUE_UP;
      }

      if (png_ptr->do_filter & PNG_FILTER_AVG)
      {
         png_ptr->avg_row = (png_bytep)png_malloc(png_ptr,
            (png_uint_32)(png_ptr->rowbytes + 1));
         png_ptr->avg_row[0] = PNG_FILTER_VALUE_AVG;
      }

      if (png_ptr->do_filter & PNG_FILTER_PAETH)
      {
         png_ptr->paeth_row = (png_bytep)png_malloc(png_ptr,
            (png_uint_32)(png_ptr->rowbytes + 1));
         png_ptr->paeth_row[0] = PNG_FILTER_VALUE_PAETH;
      }
   }
#endif /* PNG_WRITE_FILTER_SUPPORTED */

#ifdef PNG_WRITE_INTERLACING_SUPPORTED
   /* If interlaced, we need to set up width and height of pass */
   if (png_ptr->interlaced)
   {
      if (!(png_ptr->transformations & PNG_INTERLACE))
      {
         png_ptr->num_rows = (png_ptr->height + png_pass_yinc[0] - 1 -
            png_pass_ystart[0]) / png_pass_yinc[0];
         png_ptr->usr_width = (png_ptr->width + png_pass_inc[0] - 1 -
            png_pass_start[0]) / png_pass_inc[0];
      }
      else
      {
         png_ptr->num_rows = png_ptr->height;
         png_ptr->usr_width = png_ptr->width;
      }
   }
   else
#endif
   {
      png_ptr->num_rows = png_ptr->height;
      png_ptr->usr_width = png_ptr->width;
   }
   png_ptr->zstream.avail_out = (uInt)png_ptr->zbuf_size;
   png_ptr->zstream.next_out = png_ptr->zbuf;
}
