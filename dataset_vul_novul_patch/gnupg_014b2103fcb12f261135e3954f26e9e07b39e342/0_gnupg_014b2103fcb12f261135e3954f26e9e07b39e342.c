do_uncompress( compress_filter_context_t *zfx, z_stream *zs,
               IOBUF a, size_t *ret_len )
 {
     int zrc;
    int rc = 0;
    int leave = 0;
     size_t n;
     int nread, count;
     int refill = !zs->avail_in;
    if( DBG_FILTER )
	log_debug("begin inflate: avail_in=%u, avail_out=%u, inbuf=%u\n",
		(unsigned)zs->avail_in, (unsigned)zs->avail_out,
		(unsigned)zfx->inbufsize );
    do {
	if( zs->avail_in < zfx->inbufsize && refill ) {
	    n = zs->avail_in;
	    if( !n )
            zs->next_in = BYTEF_CAST (zfx->inbuf);
	    count = zfx->inbufsize - n;
	    nread = iobuf_read( a, zfx->inbuf + n, count );
            nread = iobuf_read( a, zfx->inbuf + n, count );
            if( nread == -1 ) nread = 0;
            n += nread;
           /* Algo 1 has no zlib header which requires us to to give
            * inflate an extra dummy byte to read. To be on the safe
            * side we allow for up to 4 ff bytes.  */
           if( nread < count && zfx->algo == 1 && zfx->algo1hack < 4) {
               *(zfx->inbuf + n) = 0xFF;
               zfx->algo1hack++;
                n++;
                leave = 1;
            }
            zs->avail_in = n;
        }
	    log_debug("enter inflate: avail_in=%u, avail_out=%u\n",
		    (unsigned)zs->avail_in, (unsigned)zs->avail_out);
	zrc = inflate ( zs, Z_SYNC_FLUSH );
	if( DBG_FILTER )
	    log_debug("leave inflate: avail_in=%u, avail_out=%u, zrc=%d\n",
		   (unsigned)zs->avail_in, (unsigned)zs->avail_out, zrc);
	if( zrc == Z_STREAM_END )
	    rc = -1; /* eof */
	else if( zrc != Z_OK && zrc != Z_BUF_ERROR ) {
	    if( zs->msg )
		log_fatal("zlib inflate problem: %s\n", zs->msg );
	    else
		log_fatal("zlib inflate problem: rc=%d\n", zrc );
            else
                log_fatal("zlib inflate problem: rc=%d\n", zrc );
        }
    } while (zs->avail_out && zrc != Z_STREAM_END && zrc != Z_BUF_ERROR
             && !leave);
 
     *ret_len = zfx->outbufsize - zs->avail_out;
     if( DBG_FILTER )
}
