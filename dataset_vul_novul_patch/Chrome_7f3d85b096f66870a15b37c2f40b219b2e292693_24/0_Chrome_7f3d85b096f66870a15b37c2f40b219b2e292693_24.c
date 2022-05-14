png_create_write_struct_2(png_const_charp user_png_ver, png_voidp error_ptr,
   png_error_ptr error_fn, png_error_ptr warn_fn, png_voidp mem_ptr,
   png_malloc_ptr malloc_fn, png_free_ptr free_fn)
{
#endif /* PNG_USER_MEM_SUPPORTED */
#ifdef PNG_SETJMP_SUPPORTED
   volatile
#endif
   png_structp png_ptr;
#ifdef PNG_SETJMP_SUPPORTED
#ifdef USE_FAR_KEYWORD
   jmp_buf jmpbuf;
#endif
#endif
   int i;

   png_debug(1, "in png_create_write_struct");

#ifdef PNG_USER_MEM_SUPPORTED
   png_ptr = (png_structp)png_create_struct_2(PNG_STRUCT_PNG,
      (png_malloc_ptr)malloc_fn, (png_voidp)mem_ptr);
#else
   png_ptr = (png_structp)png_create_struct(PNG_STRUCT_PNG);
#endif /* PNG_USER_MEM_SUPPORTED */
   if (png_ptr == NULL)
      return (NULL);

   /* Added at libpng-1.2.6 */
#ifdef PNG_SET_USER_LIMITS_SUPPORTED
   png_ptr->user_width_max = PNG_USER_WIDTH_MAX;
   png_ptr->user_height_max = PNG_USER_HEIGHT_MAX;
#endif

#ifdef PNG_SETJMP_SUPPORTED
#ifdef USE_FAR_KEYWORD
   if (setjmp(jmpbuf))
#else
   if (setjmp(png_ptr->jmpbuf))
#endif
   {
      png_free(png_ptr, png_ptr->zbuf);
      png_ptr->zbuf = NULL;
#ifdef PNG_USER_MEM_SUPPORTED
      png_destroy_struct_2((png_voidp)png_ptr,
         (png_free_ptr)free_fn, (png_voidp)mem_ptr);
#else
      png_destroy_struct((png_voidp)png_ptr);
#endif
      return (NULL);
   }
#ifdef USE_FAR_KEYWORD
   png_memcpy(png_ptr->jmpbuf, jmpbuf, png_sizeof(jmp_buf));
#endif
#endif

#ifdef PNG_USER_MEM_SUPPORTED
   png_set_mem_fn(png_ptr, mem_ptr, malloc_fn, free_fn);
#endif /* PNG_USER_MEM_SUPPORTED */
   png_set_error_fn(png_ptr, error_ptr, error_fn, warn_fn);

   if (user_png_ver != NULL)
   {
      int found_dots = 0;
      i = -1;

      do
      {
         i++;
         if (user_png_ver[i] != PNG_LIBPNG_VER_STRING[i])
            png_ptr->flags |= PNG_FLAG_LIBRARY_MISMATCH;
         if (user_png_ver[i] == '.')
            found_dots++;
      } while (found_dots < 2 && user_png_ver[i] != 0 &&
            PNG_LIBPNG_VER_STRING[i] != 0);
   }
   else
      png_ptr->flags |= PNG_FLAG_LIBRARY_MISMATCH;

   if (png_ptr->flags & PNG_FLAG_LIBRARY_MISMATCH)
   {
     /* Libpng 0.90 and later are binary incompatible with libpng 0.89, so
      * we must recompile any applications that use any older library version.
      * For versions after libpng 1.0, we will be compatible, so we need
      * only check the first digit.
      */
     if (user_png_ver == NULL || user_png_ver[0] != png_libpng_ver[0] ||
         (user_png_ver[0] == '1' && user_png_ver[2] != png_libpng_ver[2]) ||
         (user_png_ver[0] == '0' && user_png_ver[2] < '9'))
     {
#if defined(PNG_STDIO_SUPPORTED) && !defined(_WIN32_WCE)
        char msg[80];
        if (user_png_ver)
        {
           png_snprintf(msg, 80,
              "Application was compiled with png.h from libpng-%.20s",
              user_png_ver);
           png_warning(png_ptr, msg);
        }
        png_snprintf(msg, 80,
           "Application  is  running with png.c from libpng-%.20s",
           png_libpng_ver);
        png_warning(png_ptr, msg);
#endif
#ifdef PNG_ERROR_NUMBERS_SUPPORTED
        png_ptr->flags = 0;
#endif
        png_error(png_ptr,
           "Incompatible libpng version in application and library");
     }
   }

   /* Initialize zbuf - compression buffer */
   png_ptr->zbuf_size = PNG_ZBUF_SIZE;
   png_ptr->zbuf = (png_bytep)png_malloc(png_ptr,
     (png_uint_32)png_ptr->zbuf_size);

    png_set_write_fn(png_ptr, png_voidp_NULL, png_rw_ptr_NULL,
       png_flush_ptr_NULL);
 
 #ifdef PNG_SETJMP_SUPPORTED
    /* Applications that neglect to set up their own setjmp() and then
     * encounter a png_error() will longjmp here.  Since the jmpbuf is
    * then meaningless we abort instead of returning.
    */
#ifdef USE_FAR_KEYWORD
   if (setjmp(jmpbuf))
      PNG_ABORT();
   png_memcpy(png_ptr->jmpbuf, jmpbuf, png_sizeof(jmp_buf));
#else
   if (setjmp(png_ptr->jmpbuf))
      PNG_ABORT();
#endif
#endif
   return (png_ptr);
}
