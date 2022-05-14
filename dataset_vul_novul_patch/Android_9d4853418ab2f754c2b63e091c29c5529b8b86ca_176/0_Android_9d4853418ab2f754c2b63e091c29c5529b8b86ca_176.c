zlib_init(struct zlib *zlib, struct IDAT *idat, struct chunk *chunk,
 int window_bits, png_uint_32 offset)
 /* Initialize a zlib_control; the result is true/false */
{
   CLEAR(*zlib);

   zlib->idat = idat;
   zlib->chunk = chunk;
   zlib->file = chunk->file;
   zlib->global = chunk->global;
   zlib->rewrite_offset = offset; /* never changed for this zlib */

 /* *_out does not need to be set: */
   zlib->z.next_in = Z_NULL;
   zlib->z.avail_in = 0;
   zlib->z.zalloc = Z_NULL;
   zlib->z.zfree = Z_NULL;
   zlib->z.opaque = Z_NULL;

   zlib->state = -1;
   zlib->window_bits = window_bits;

   zlib->compressed_digits = 0;
   zlib->uncompressed_digits = 0;


    /* These values are sticky across reset (in addition to the stuff in the
     * first block, which is actually constant.)
     */
   zlib->file_bits = 24;
    zlib->ok_bits = 16; /* unset */
    zlib->cksum = 0; /* set when a checksum error is detected */
 
 /* '0' means use the header; inflateInit2 should always succeed because it
    * does nothing apart from allocating the internal zstate.
    */
   zlib->rc = inflateInit2(&zlib->z, 0);
 if (zlib->rc != Z_OK)
 {
      zlib_message(zlib, 1/*unexpected*/);
 return 0;
 }

 else
 {
      zlib->state = 0; /* initialized */
 return 1;
 }
}
