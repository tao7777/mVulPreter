 header_put_byte (SF_PRIVATE *psf, char x)
{	if (psf->headindex < SIGNED_SIZEOF (psf->header) - 1)
		psf->header [psf->headindex++] = x ;
 } /* header_put_byte */
