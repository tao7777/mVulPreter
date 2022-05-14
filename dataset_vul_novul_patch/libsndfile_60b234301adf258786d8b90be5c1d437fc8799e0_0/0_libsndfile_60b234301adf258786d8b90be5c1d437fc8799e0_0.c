 flac_read_loop (SF_PRIVATE *psf, unsigned len)
 {	FLAC_PRIVATE* pflac = (FLAC_PRIVATE*) psf->codec_data ;
	FLAC__StreamDecoderState state ;
 
 	pflac->pos = 0 ;
 	pflac->len = len ;
 	pflac->remain = len ;
 
	state = FLAC__stream_decoder_get_state (pflac->fsd) ;
	if (state > FLAC__STREAM_DECODER_END_OF_STREAM)
	{	psf_log_printf (psf, "FLAC__stream_decoder_get_state returned %s\n", FLAC__StreamDecoderStateString [state]) ;
		/* Current frame is busted, so NULL the pointer. */
		pflac->frame = NULL ;
		} ;

 	/* First copy data that has already been decoded and buffered. */
 	if (pflac->frame != NULL && pflac->bufferpos < pflac->frame->header.blocksize)
 		flac_buffer_copy (psf) ;

	/* Decode some more. */
 	while (pflac->pos < pflac->len)
 	{	if (FLAC__stream_decoder_process_single (pflac->fsd) == 0)
 			break ;
		state = FLAC__stream_decoder_get_state (pflac->fsd) ;
		if (state >= FLAC__STREAM_DECODER_END_OF_STREAM)
		{	psf_log_printf (psf, "FLAC__stream_decoder_get_state returned %s\n", FLAC__StreamDecoderStateString [state]) ;
			/* Current frame is busted, so NULL the pointer. */
			pflac->frame = NULL ;
 			break ;
			} ;
 		} ;
 
 	pflac->ptr = NULL ;

	return pflac->pos ;
} /* flac_read_loop */
