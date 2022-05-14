 header_put_le_short (SF_PRIVATE *psf, int x)
{	if (psf->headindex < SIGNED_SIZEOF (psf->header) - 2)
	{	psf->header [psf->headindex++] = x ;
		psf->header [psf->headindex++] = (x >> 8) ;
		} ;
 } /* header_put_le_short */
