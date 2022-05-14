 header_put_be_3byte (SF_PRIVATE *psf, int x)
{	if (psf->headindex < SIGNED_SIZEOF (psf->header) - 3)
	{	psf->header [psf->headindex++] = (x >> 16) ;
		psf->header [psf->headindex++] = (x >> 8) ;
		psf->header [psf->headindex++] = x ;
		} ;
 } /* header_put_be_3byte */
