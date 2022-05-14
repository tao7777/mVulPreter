 header_put_le_short (SF_PRIVATE *psf, int x)
{	psf->header.ptr [psf->header.indx++] = x ;
	psf->header.ptr [psf->header.indx++] = (x >> 8) ;
 } /* header_put_le_short */
