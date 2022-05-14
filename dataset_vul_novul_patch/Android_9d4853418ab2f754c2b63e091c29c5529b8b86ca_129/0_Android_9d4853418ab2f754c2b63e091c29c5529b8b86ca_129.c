 set_store_for_read(png_store *ps, png_infopp ppi, png_uint_32 id,
   const char *name)
 {
    /* Set the name for png_error */
    safecat(ps->test, sizeof ps->test, 0, name);

 if (ps->pread != NULL)
      png_error(ps->pread, "read store already in use");

   store_read_reset(ps);

 /* Both the create APIs can return NULL if used in their default mode
    * (because there is no other way of handling an error because the jmp_buf
    * by default is stored in png_struct and that has not been allocated!)
    * However, given that store_error works correctly in these circumstances
    * we don't ever expect NULL in this program.
    */
#  ifdef PNG_USER_MEM_SUPPORTED
 if (!ps->speed)
         ps->pread = png_create_read_struct_2(PNG_LIBPNG_VER_STRING, ps,
             store_error, store_warning, &ps->read_memory_pool, store_malloc,
             store_free);

 else
#  endif
   ps->pread = png_create_read_struct(PNG_LIBPNG_VER_STRING, ps, store_error,
      store_warning);

 if (ps->pread == NULL)
 {
 struct exception_context *the_exception_context = &ps->exception_context;

      store_log(ps, NULL, "png_create_read_struct returned NULL (unexpected)",
 1 /*error*/);

 Throw ps;
 }

#  ifdef PNG_SET_OPTION_SUPPORTED
 {
 int opt;
 for (opt=0; opt<ps->noptions; ++opt)
 if (png_set_option(ps->pread, ps->options[opt].option,
               ps->options[opt].setting) == PNG_OPTION_INVALID)
                  png_error(ps->pread, "png option invalid");
 }
#  endif

   store_read_set(ps, id);

 if (ppi != NULL)
 *ppi = ps->piread = png_create_info_struct(ps->pread);

 return ps->pread;
}
