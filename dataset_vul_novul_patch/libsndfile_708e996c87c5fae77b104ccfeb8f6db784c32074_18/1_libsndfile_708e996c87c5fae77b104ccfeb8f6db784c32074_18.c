psf_close (SF_PRIVATE *psf)
{	uint32_t k ;
	int	error = 0 ;

	if (psf->codec_close)
	{	error = psf->codec_close (psf) ;
		/* To prevent it being called in psf->container_close(). */
		psf->codec_close = NULL ;
		} ;

	if (psf->container_close)
		error = psf->container_close (psf) ;

	error = psf_fclose (psf) ;
 	psf_close_rsrc (psf) ;
 
 	/* For an ISO C compliant implementation it is ok to free a NULL pointer. */
 	free (psf->container_data) ;
 	free (psf->codec_data) ;
 	free (psf->interleave) ;
	free (psf->dither) ;
	free (psf->peak_info) ;
	free (psf->broadcast_16k) ;
	free (psf->loop_info) ;
	free (psf->instrument) ;
	free (psf->cues) ;
	free (psf->channel_map) ;
	free (psf->format_desc) ;
	free (psf->strings.storage) ;

	if (psf->wchunks.chunks)
		for (k = 0 ; k < psf->wchunks.used ; k++)
			free (psf->wchunks.chunks [k].data) ;
	free (psf->rchunks.chunks) ;
	free (psf->wchunks.chunks) ;
	free (psf->iterator) ;
	free (psf->cart_16k) ;

	memset (psf, 0, sizeof (SF_PRIVATE)) ;
	free (psf) ;

	return error ;
} /* psf_close */
