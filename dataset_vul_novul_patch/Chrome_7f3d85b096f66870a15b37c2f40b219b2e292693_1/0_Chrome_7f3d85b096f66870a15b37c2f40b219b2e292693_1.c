png_info_init_3(png_infopp ptr_ptr, png_size_t png_info_struct_size)
{
   png_infop info_ptr = *ptr_ptr;

   png_debug(1, "in png_info_init_3");

   if (info_ptr == NULL)
      return;

   if (png_sizeof(png_info) > png_info_struct_size)
   {
       png_destroy_struct(info_ptr);
       info_ptr = (png_infop)png_create_struct(PNG_STRUCT_INFO);
       *ptr_ptr = info_ptr;
      if (info_ptr == NULL)
         return;
    }
 
    /* Set everything to 0 */
   png_memset(info_ptr, 0, png_sizeof(png_info));
}
