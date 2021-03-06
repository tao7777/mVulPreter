process_IDAT(struct file *file)
 /* Process the IDAT stream, this is the more complex than the preceding
    * cases because the compressed data is spread across multiple IDAT chunks
    * (typically).  Rechunking of the data is not handled here; all this
    * function does is establish whether the zlib header needs to be modified.
    *
    * Initially the function returns false, indicating that the chunk should not
    * be written.  It does this until the last IDAT chunk is passed in, then it
    * checks the zlib data and returns true.
    *
    * It does not return false on a fatal error; it calls stop instead.
    *
    * The caller must have an instantiated (IDAT) control structure and it must
    * have extent over the whole read of the IDAT stream.  For a PNG this means
    * the whole PNG read, for MNG it could have lesser extent.
    */
{
 struct IDAT_list *list;

   assert(file->idat != NULL && file->chunk != NULL);

 /* We need to first check the entire sequence of IDAT chunks to ensure the
    * stream is in sync.  Do this by building a list of all the chunks and
    * recording the length of each because the length may have been fixed up by
    * sync_stream below.
    *
    * At the end of the list of chunks, where the type of the next chunk is not
    * png_IDAT, process the whole stream using the list data to check validity
    * then return control to the start and rewrite everything.
    */
 list = file->idat->idat_list_tail;

 if (list->count == list->length)
 {
 list = IDAT_list_extend(list);

 if (list == NULL)
         stop(file, READ_ERROR_CODE, "out of memory");

 /* Move to the next block */

       list->count = 0;
       file->idat->idat_list_tail = list;
    }

    /* And fill in the next IDAT information buffer. */
    list->lengths[(list->count)++] = file->chunk->chunk_length;
 
 /* The type of the next chunk was recorded in the file control structure by
    * the caller, if this is png_IDAT return 'skip' to the caller.
    */
 if (file->type == png_IDAT)
 return 0; /* skip this for the moment */

 /* This is the final IDAT chunk, so run the tests to check for the too far
    * back error and possibly optimize the window bits.  This means going back
    * to the start of the first chunk data, which is stored in the original
    * chunk allocation.
    */
   setpos(file->chunk);

 if (zlib_check(file, 0))
 {
 struct IDAT *idat;
 int cmp;

 /* The IDAT stream was successfully uncompressed; see whether it
       * contained the correct number of bytes of image data.
       */
      cmp = uarb_cmp(file->image_bytes, file->image_digits,
         file->chunk->uncompressed_bytes, file->chunk->uncompressed_digits);

 if (cmp < 0)
         type_message(file, png_IDAT, "extra uncompressed data");

 else if (cmp > 0)
         stop(file, LIBPNG_ERROR_CODE, "IDAT: uncompressed data too small");

 /* Return the stream to the start of the first IDAT chunk; the length
       * is set in the write case below but the input chunk variables must be
       * set (once) here:
       */
      setpos(file->chunk);

      idat = file->idat;
      idat->idat_cur = idat->idat_list_head;
      idat->idat_length = idat->idat_cur->lengths[0];
      idat->idat_count = 0; /* Count of chunks read in current list */
      idat->idat_index = 0; /* Index into chunk data */

 /* Update the chunk length to the correct value for the IDAT chunk: */
      file->chunk->chunk_length = rechunk_length(idat);

 /* Change the state to writing IDAT chunks */
      file->state = STATE_IDAT;

 return 1;
 }

 else /* Failure to decompress the IDAT stream; give up. */
      stop(file, ZLIB_ERROR_CODE, "could not uncompress IDAT");
}
