png_write_PLTE(png_structp png_ptr, png_colorp palette, png_uint_32 num_pal)
{
 #ifdef PNG_USE_LOCAL_ARRAYS
    PNG_PLTE;
 #endif
   png_uint_32 max_palette_length, i;
    png_colorp pal_ptr;
    png_byte buf[3];
 
    png_debug(1, "in png_write_PLTE");
 
   max_palette_length = (png_ptr->color_type == PNG_COLOR_TYPE_PALETTE) ?
      (1 << png_ptr->bit_depth) : PNG_MAX_PALETTE_LENGTH;

    if ((
 #ifdef PNG_MNG_FEATURES_SUPPORTED
         !(png_ptr->mng_features_permitted & PNG_FLAG_MNG_EMPTY_PLTE) &&
 #endif
       num_pal == 0) || num_pal > max_palette_length)
    {
      if (png_ptr->color_type == PNG_COLOR_TYPE_PALETTE)
      {
        png_error(png_ptr, "Invalid number of colors in palette");
     }
     else
     {
        png_warning(png_ptr, "Invalid number of colors in palette");
        return;
     }
   }

   if (!(png_ptr->color_type&PNG_COLOR_MASK_COLOR))
   {
      png_warning(png_ptr,
        "Ignoring request to write a PLTE chunk in grayscale PNG");
      return;
   }

   png_ptr->num_palette = (png_uint_16)num_pal;
   png_debug1(3, "num_palette = %d", png_ptr->num_palette);

   png_write_chunk_start(png_ptr, (png_bytep)png_PLTE,
     (png_uint_32)(num_pal * 3));
#ifdef PNG_POINTER_INDEXING_SUPPORTED
   for (i = 0, pal_ptr = palette; i < num_pal; i++, pal_ptr++)
   {
      buf[0] = pal_ptr->red;
      buf[1] = pal_ptr->green;
      buf[2] = pal_ptr->blue;
      png_write_chunk_data(png_ptr, buf, (png_size_t)3);
   }
#else
   /* This is a little slower but some buggy compilers need to do this
    * instead
    */
   pal_ptr=palette;
   for (i = 0; i < num_pal; i++)
   {
      buf[0] = pal_ptr[i].red;
      buf[1] = pal_ptr[i].green;
      buf[2] = pal_ptr[i].blue;
      png_write_chunk_data(png_ptr, buf, (png_size_t)3);
   }
#endif
   png_write_chunk_end(png_ptr);
   png_ptr->mode |= PNG_HAVE_PLTE;
}
