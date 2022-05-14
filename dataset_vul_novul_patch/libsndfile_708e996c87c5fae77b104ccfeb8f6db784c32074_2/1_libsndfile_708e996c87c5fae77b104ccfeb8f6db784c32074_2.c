 header_put_be_8byte (SF_PRIVATE *psf, sf_count_t x)
{	if (psf->headindex < SIGNED_SIZEOF (psf->header) - 8)
	{	psf->header [psf->headindex++] = (x >> 56) ;
		psf->header [psf->headindex++] = (x >> 48) ;
		psf->header [psf->headindex++] = (x >> 40) ;
		psf->header [psf->headindex++] = (x >> 32) ;
		psf->header [psf->headindex++] = (x >> 24) ;
		psf->header [psf->headindex++] = (x >> 16) ;
		psf->header [psf->headindex++] = (x >> 8) ;
		psf->header [psf->headindex++] = x ;
		} ;
 } /* header_put_be_8byte */
