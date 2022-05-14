flac_buffer_copy (SF_PRIVATE *psf)
{	FLAC_PRIVATE* pflac = (FLAC_PRIVATE*) psf->codec_data ;
	const FLAC__Frame *frame = pflac->frame ;
	const int32_t* const *buffer = pflac->wbuffer ;
	unsigned i = 0, j, offset, channels, len ;

	/*
	**	frame->header.blocksize is variable and we're using a constant blocksize
	**	of FLAC__MAX_BLOCK_SIZE.
	**	Check our assumptions here.
	*/
	if (frame->header.blocksize > FLAC__MAX_BLOCK_SIZE)
	{	psf_log_printf (psf, "Ooops : frame->header.blocksize (%d) > FLAC__MAX_BLOCK_SIZE (%d)\n", __func__, __LINE__, frame->header.blocksize, FLAC__MAX_BLOCK_SIZE) ;
		psf->error = SFE_INTERNAL ;
		return 0 ;
		} ;

	if (frame->header.channels > FLAC__MAX_CHANNELS)
		psf_log_printf (psf, "Ooops : frame->header.channels (%d) > FLAC__MAX_BLOCK_SIZE (%d)\n", __func__, __LINE__, frame->header.channels, FLAC__MAX_CHANNELS) ;

	channels = SF_MIN (frame->header.channels, FLAC__MAX_CHANNELS) ;
 
 	if (pflac->ptr == NULL)
 	{	/*
		** This pointer is reset to NULL each time the current frame has been
		** decoded. Somehow its used during encoding and decoding.
 		*/
 		for (i = 0 ; i < channels ; i++)
 		{
 			if (pflac->rbuffer [i] == NULL)
				pflac->rbuffer [i] = calloc (FLAC__MAX_BLOCK_SIZE, sizeof (int32_t)) ;

			memcpy (pflac->rbuffer [i], buffer [i], frame->header.blocksize * sizeof (int32_t)) ;
			} ;
		pflac->wbuffer = (const int32_t* const*) pflac->rbuffer ;

		return 0 ;
		} ;

 
 	len = SF_MIN (pflac->len, frame->header.blocksize) ;
 
	if (pflac->remain % channels != 0)
	{	psf_log_printf (psf, "Error: pflac->remain %u    channels %u\n", pflac->remain, channels) ;
		return 0 ;
		} ;

 	switch (pflac->pcmtype)
 	{	case PFLAC_PCM_SHORT :
 			{	short *retpcm = (short*) pflac->ptr ;
				int shift = 16 - frame->header.bits_per_sample ;
				if (shift < 0)
				{	shift = abs (shift) ;
					for (i = 0 ; i < len && pflac->remain > 0 ; i++)
					{	offset = pflac->pos + i * channels ;

						if (pflac->bufferpos >= frame->header.blocksize)
							break ;

						if (offset + channels > pflac->len)
							break ;

						for (j = 0 ; j < channels ; j++)
							retpcm [offset + j] = buffer [j][pflac->bufferpos] >> shift ;
						pflac->remain -= channels ;
						pflac->bufferpos++ ;
						}
					}
				else
				{	for (i = 0 ; i < len && pflac->remain > 0 ; i++)
					{	offset = pflac->pos + i * channels ;

						if (pflac->bufferpos >= frame->header.blocksize)
							break ;

						if (offset + channels > pflac->len)
							break ;

						for (j = 0 ; j < channels ; j++)
							retpcm [offset + j] = ((uint16_t) buffer [j][pflac->bufferpos]) << shift ;

						pflac->remain -= channels ;
						pflac->bufferpos++ ;
						} ;
					} ;
				} ;
			break ;

		case PFLAC_PCM_INT :
			{	int *retpcm = (int*) pflac->ptr ;
				int shift = 32 - frame->header.bits_per_sample ;
				for (i = 0 ; i < len && pflac->remain > 0 ; i++)
				{	offset = pflac->pos + i * channels ;

					if (pflac->bufferpos >= frame->header.blocksize)
						break ;

					if (offset + channels > pflac->len)
						break ;

					for (j = 0 ; j < channels ; j++)
						retpcm [offset + j] = ((uint32_t) buffer [j][pflac->bufferpos]) << shift ;
					pflac->remain -= channels ;
					pflac->bufferpos++ ;
					} ;
				} ;
			break ;

		case PFLAC_PCM_FLOAT :
			{	float *retpcm = (float*) pflac->ptr ;
				float norm = (psf->norm_float == SF_TRUE) ? 1.0 / (1 << (frame->header.bits_per_sample - 1)) : 1.0 ;

				for (i = 0 ; i < len && pflac->remain > 0 ; i++)
				{	offset = pflac->pos + i * channels ;

					if (pflac->bufferpos >= frame->header.blocksize)
						break ;

					if (offset + channels > pflac->len)
						break ;

					for (j = 0 ; j < channels ; j++)
						retpcm [offset + j] = buffer [j][pflac->bufferpos] * norm ;
					pflac->remain -= channels ;
					pflac->bufferpos++ ;
					} ;
				} ;
			break ;

		case PFLAC_PCM_DOUBLE :
			{	double *retpcm = (double*) pflac->ptr ;
				double norm = (psf->norm_double == SF_TRUE) ? 1.0 / (1 << (frame->header.bits_per_sample - 1)) : 1.0 ;

				for (i = 0 ; i < len && pflac->remain > 0 ; i++)
				{	offset = pflac->pos + i * channels ;

					if (pflac->bufferpos >= frame->header.blocksize)
						break ;

					if (offset + channels > pflac->len)
						break ;

					for (j = 0 ; j < channels ; j++)
						retpcm [offset + j] = buffer [j][pflac->bufferpos] * norm ;
					pflac->remain -= channels ;
					pflac->bufferpos++ ;
					} ;
				} ;
			break ;

		default :
			return 0 ;
		} ;

	offset = i * channels ;
	pflac->pos += i * channels ;

	return offset ;
} /* flac_buffer_copy */
