png_read_init_3(png_structpp ptr_ptr, png_const_charp user_png_ver,
   png_size_t png_struct_size)
{
#ifdef PNG_SETJMP_SUPPORTED
   jmp_buf tmp_jmp;  /* to save current jump buffer */
#endif

   int i = 0;

   png_structp png_ptr=*ptr_ptr;

   if (png_ptr == NULL)
      return;
 
    do
    {
      if (user_png_ver == NULL || user_png_ver[i] != png_libpng_ver[i])
       {
 #ifdef PNG_LEGACY_SUPPORTED
         png_ptr->flags |= PNG_FLAG_LIBRARY_MISMATCH;
#else
        png_ptr->warning_fn = NULL;
        png_warning(png_ptr,
         "Application uses deprecated png_read_init() and should be"
         " recompiled.");
        break;
#endif
      }
   } while (png_libpng_ver[i++]);

   png_debug(1, "in png_read_init_3");

#ifdef PNG_SETJMP_SUPPORTED
   /* Save jump buffer and error functions */
   png_memcpy(tmp_jmp, png_ptr->jmpbuf, png_sizeof(jmp_buf));
#endif

   if (png_sizeof(png_struct) > png_struct_size)
   {
      png_destroy_struct(png_ptr);
      *ptr_ptr = (png_structp)png_create_struct(PNG_STRUCT_PNG);
      png_ptr = *ptr_ptr;
   }

   /* Reset all variables to 0 */
   png_memset(png_ptr, 0, png_sizeof(png_struct));

#ifdef PNG_SETJMP_SUPPORTED
   /* Restore jump buffer */
   png_memcpy(png_ptr->jmpbuf, tmp_jmp, png_sizeof(jmp_buf));
#endif

   /* Added at libpng-1.2.6 */
#ifdef PNG_SET_USER_LIMITS_SUPPORTED
   png_ptr->user_width_max = PNG_USER_WIDTH_MAX;
   png_ptr->user_height_max = PNG_USER_HEIGHT_MAX;
#endif

   /* Initialize zbuf - compression buffer */
   png_ptr->zbuf_size = PNG_ZBUF_SIZE;
   png_ptr->zstream.zalloc = png_zalloc;
   png_ptr->zbuf = (png_bytep)png_malloc(png_ptr,
     (png_uint_32)png_ptr->zbuf_size);
   png_ptr->zstream.zalloc = png_zalloc;
   png_ptr->zstream.zfree = png_zfree;
   png_ptr->zstream.opaque = (voidpf)png_ptr;

   switch (inflateInit(&png_ptr->zstream))
   {
      case Z_OK: /* Do nothing */ break;
      case Z_STREAM_ERROR: png_error(png_ptr, "zlib memory error"); break;
      case Z_VERSION_ERROR: png_error(png_ptr, "zlib version error");
          break;
      default: png_error(png_ptr, "Unknown zlib error");
   }

   png_ptr->zstream.next_out = png_ptr->zbuf;
   png_ptr->zstream.avail_out = (uInt)png_ptr->zbuf_size;

   png_set_read_fn(png_ptr, png_voidp_NULL, png_rw_ptr_NULL);
}
