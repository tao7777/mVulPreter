 header_put_be_short (SF_PRIVATE *psf, int x)
{	if (psf->headindex < SIGNED_SIZEOF (psf->header) - 2)
	{	psf->header [psf->headindex++] = (x >> 8) ;
		psf->header [psf->headindex++] = x ;
		} ;
 } /* header_put_be_short */
