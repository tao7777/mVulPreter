read_callback(png_structp pp, png_unknown_chunkp pc)
{
 /* This function mimics the behavior of png_set_keep_unknown_chunks by
    * returning '0' to keep the chunk and '1' to discard it.
    */
   display *d = voidcast(display*, png_get_user_chunk_ptr(pp));
 int chunk = findb(pc->name);
 int keep, discard;

 if (chunk < 0) /* not one in our list, so not a known chunk */
      keep = d->keep;

 else
 {
      keep = chunk_info[chunk].keep;
 if (keep == PNG_HANDLE_CHUNK_AS_DEFAULT)
 {
 /* See the comments in png.h - use the default for unknown chunks,
          * do not keep known chunks.
          */
 if (chunk_info[chunk].unknown)
            keep = d->keep;

 else
            keep = PNG_HANDLE_CHUNK_NEVER;
 }
 }

 switch (keep)
 {
 default:
         fprintf(stderr, "%s(%s): %d: unrecognized chunk option\n", d->file,
            d->test, chunk_info[chunk].keep);
         display_exit(d);

 case PNG_HANDLE_CHUNK_AS_DEFAULT:
 case PNG_HANDLE_CHUNK_NEVER:
         discard = 1/*handled; discard*/;
 break;

 case PNG_HANDLE_CHUNK_IF_SAFE:
 case PNG_HANDLE_CHUNK_ALWAYS:
         discard = 0/*not handled; keep*/;
 break;
 }

 /* Also store information about this chunk in the display, the relevant flag
    * is set if the chunk is to be kept ('not handled'.)
    */
 if (chunk >= 0) if (!discard) /* stupidity to stop a GCC warning */
 {
      png_uint_32 flag = chunk_info[chunk].flag;

 if (pc->location & PNG_AFTER_IDAT)
         d->after_IDAT |= flag;

 else
         d->before_IDAT |= flag;
 }


    /* However if there is no support to store unknown chunks don't ask libpng to
     * do it; there will be an png_error.
     */
#  ifdef PNG_STORE_UNKNOWN_CHUNKS_SUPPORTED
       return discard;
 #  else
       return 1; /*handled; discard*/
#  endif

 }
