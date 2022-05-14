set_store_for_write(png_store *ps, png_infopp ppi,
set_store_for_write(png_store *ps, png_infopp ppi, const char *name)
 {
    anon_context(ps);
 
 Try
 {
 if (ps->pwrite != NULL)
         png_error(ps->pwrite, "write store already in use");

      store_write_reset(ps);
      safecat(ps->wname, sizeof ps->wname, 0, name);

 /* Don't do the slow memory checks if doing a speed test, also if user
       * memory is not supported we can't do it anyway.
       */
#     ifdef PNG_USER_MEM_SUPPORTED
 if (!ps->speed)
            ps->pwrite = png_create_write_struct_2(PNG_LIBPNG_VER_STRING,
               ps, store_error, store_warning, &ps->write_memory_pool,
               store_malloc, store_free);

 else
#     endif
         ps->pwrite = png_create_write_struct(PNG_LIBPNG_VER_STRING,
            ps, store_error, store_warning);

      png_set_write_fn(ps->pwrite, ps, store_write, store_flush);

#     ifdef PNG_SET_OPTION_SUPPORTED
 {
 int opt;
 for (opt=0; opt<ps->noptions; ++opt)
 if (png_set_option(ps->pwrite, ps->options[opt].option,
                  ps->options[opt].setting) == PNG_OPTION_INVALID)
                  png_error(ps->pwrite, "png option invalid");
 }
#     endif

 if (ppi != NULL)
 *ppi = ps->piwrite = png_create_info_struct(ps->pwrite);
 }

 Catch_anonymous
 return NULL;

 return ps->pwrite;
}
