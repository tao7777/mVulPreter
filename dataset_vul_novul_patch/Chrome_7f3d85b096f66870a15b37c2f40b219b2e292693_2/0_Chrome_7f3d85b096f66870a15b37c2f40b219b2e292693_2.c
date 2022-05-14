 png_zalloc(voidpf png_ptr, uInt items, uInt size)
 {
    png_voidp ptr;
   png_structp p;
   png_uint_32 save_flags;
    png_uint_32 num_bytes;
 
    if (png_ptr == NULL)
       return (NULL);

   p=(png_structp)png_ptr;
   save_flags=p->flags;

    if (items > PNG_UINT_32_MAX/size)
    {
      png_warning (p, "Potential overflow in png_zalloc()");
     return (NULL);
   }
   num_bytes = (png_uint_32)items * size;

   p->flags|=PNG_FLAG_MALLOC_NULL_MEM_OK;
   ptr = (png_voidp)png_malloc((png_structp)png_ptr, num_bytes);
   p->flags=save_flags;

#if defined(PNG_1_0_X) && !defined(PNG_NO_ZALLOC_ZERO)
   if (ptr == NULL)
       return ((voidpf)ptr);

   if (num_bytes > (png_uint_32)0x8000L)
   {
      png_memset(ptr, 0, (png_size_t)0x8000L);
      png_memset((png_bytep)ptr + (png_size_t)0x8000L, 0,
         (png_size_t)(num_bytes - (png_uint_32)0x8000L));
   }
   else
   {
      png_memset(ptr, 0, (png_size_t)num_bytes);
   }
#endif
   return ((voidpf)ptr);
}
