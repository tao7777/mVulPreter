store_pool_error(png_store *ps, png_const_structp pp, PNG_CONST char *msg)
store_pool_error(png_store *ps, png_const_structp pp, const char *msg)
 {
    if (pp != NULL)
       png_error(pp, msg);

 /* Else we have to do it ourselves.  png_error eventually calls store_log,
    * above.  store_log accepts a NULL png_structp - it just changes what gets
    * output by store_message.
    */
   store_log(ps, pp, msg, 1 /* error */);
}
