 header_put_le_3byte (SF_PRIVATE *psf, int x)
{	if (psf->headindex < SIGNED_SIZEOF (psf->header) - 3)
	{	psf->header [psf->headindex++] = x ;
		psf->header [psf->headindex++] = (x >> 8) ;
		psf->header [psf->headindex++] = (x >> 16) ;
		} ;
 } /* header_put_le_3byte */
