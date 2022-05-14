 header_put_le_int (SF_PRIVATE *psf, int x)
{	psf->header.ptr [psf->header.indx++] = x ;
	psf->header.ptr [psf->header.indx++] = (x >> 8) ;
	psf->header.ptr [psf->header.indx++] = (x >> 16) ;
	psf->header.ptr [psf->header.indx++] = (x >> 24) ;
 } /* header_put_le_int */
