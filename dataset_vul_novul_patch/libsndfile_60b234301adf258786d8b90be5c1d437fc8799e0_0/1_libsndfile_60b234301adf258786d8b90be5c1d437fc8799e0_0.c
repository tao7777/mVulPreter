 flac_read_loop (SF_PRIVATE *psf, unsigned len)
 {	FLAC_PRIVATE* pflac = (FLAC_PRIVATE*) psf->codec_data ;
 
 	pflac->pos = 0 ;
 	pflac->len = len ;
 	pflac->remain = len ;
 
 	/* First copy data that has already been decoded and buffered. */
 	if (pflac->frame != NULL && pflac->bufferpos < pflac->frame->header.blocksize)
 		flac_buffer_copy (psf) ;

	/* Decode some more. */
 	while (pflac->pos < pflac->len)
 	{	if (FLAC__stream_decoder_process_single (pflac->fsd) == 0)
 			break ;
		if (FLAC__stream_decoder_get_state (pflac->fsd) >= FLAC__STREAM_DECODER_END_OF_STREAM)
 			break ;
 		} ;
 
 	pflac->ptr = NULL ;

	return pflac->pos ;
} /* flac_read_loop */
