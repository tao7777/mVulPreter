 header_put_be_short (SF_PRIVATE *psf, int x)
{	psf->header.ptr [psf->header.indx++] = (x >> 8) ;
	psf->header.ptr [psf->header.indx++] = x ;
 } /* header_put_be_short */
