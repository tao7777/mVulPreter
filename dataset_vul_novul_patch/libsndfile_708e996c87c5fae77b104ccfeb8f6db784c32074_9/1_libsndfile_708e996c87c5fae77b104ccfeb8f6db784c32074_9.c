 header_put_le_int (SF_PRIVATE *psf, int x)
{	if (psf->headindex < SIGNED_SIZEOF (psf->header) - 4)
	{	psf->header [psf->headindex++] = x ;
		psf->header [psf->headindex++] = (x >> 8) ;
		psf->header [psf->headindex++] = (x >> 16) ;
		psf->header [psf->headindex++] = (x >> 24) ;
		} ;
 } /* header_put_le_int */
