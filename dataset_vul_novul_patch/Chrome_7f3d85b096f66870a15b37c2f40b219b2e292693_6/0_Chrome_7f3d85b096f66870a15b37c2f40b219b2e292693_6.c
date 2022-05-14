png_push_save_buffer(png_structp png_ptr)
{
   if (png_ptr->save_buffer_size)
   {
      if (png_ptr->save_buffer_ptr != png_ptr->save_buffer)
      {
         png_size_t i, istop;
         png_bytep sp;
         png_bytep dp;

         istop = png_ptr->save_buffer_size;
         for (i = 0, sp = png_ptr->save_buffer_ptr, dp = png_ptr->save_buffer;
            i < istop; i++, sp++, dp++)
         {
            *dp = *sp;
         }
      }
   }
   if (png_ptr->save_buffer_size + png_ptr->current_buffer_size >
      png_ptr->save_buffer_max)
   {
      png_size_t new_max;
      png_bytep old_buffer;

      if (png_ptr->save_buffer_size > PNG_SIZE_MAX -
         (png_ptr->current_buffer_size + 256))
      {
        png_error(png_ptr, "Potential overflow of save_buffer");
      }

      new_max = png_ptr->save_buffer_size + png_ptr->current_buffer_size + 256;
      old_buffer = png_ptr->save_buffer;
      png_ptr->save_buffer = (png_bytep)png_malloc_warn(png_ptr,
         (png_uint_32)new_max);
      if (png_ptr->save_buffer == NULL)
      {
         png_free(png_ptr, old_buffer);
         png_error(png_ptr, "Insufficient memory for save_buffer");
       }
      else
      {
        png_memcpy(png_ptr->save_buffer, old_buffer, png_ptr->save_buffer_size);
        png_free(png_ptr, old_buffer);
        png_ptr->save_buffer_max = new_max;
      }
    }
    if (png_ptr->current_buffer_size)
    {
      png_memcpy(png_ptr->save_buffer + png_ptr->save_buffer_size,
         png_ptr->current_buffer_ptr, png_ptr->current_buffer_size);
      png_ptr->save_buffer_size += png_ptr->current_buffer_size;
      png_ptr->current_buffer_size = 0;
   }
   png_ptr->save_buffer_ptr = png_ptr->save_buffer;
   png_ptr->buffer_size = 0;
}
