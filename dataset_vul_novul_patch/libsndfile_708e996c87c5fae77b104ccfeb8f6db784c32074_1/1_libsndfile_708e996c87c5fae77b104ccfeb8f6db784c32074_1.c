header_put_be_8byte (SF_PRIVATE *psf, sf_count_t x)
{	if (psf->headindex < SIGNED_SIZEOF (psf->header) - 8)
	{	psf->header [psf->headindex++] = 0 ;
		psf->header [psf->headindex++] = 0 ;
		psf->header [psf->headindex++] = 0 ;
		psf->header [psf->headindex++] = 0 ;
		psf->header [psf->headindex++] = (x >> 24) ;
		psf->header [psf->headindex++] = (x >> 16) ;
		psf->header [psf->headindex++] = (x >> 8) ;
		psf->header [psf->headindex++] = x ;
		} ;
} /* header_put_be_8byte */
