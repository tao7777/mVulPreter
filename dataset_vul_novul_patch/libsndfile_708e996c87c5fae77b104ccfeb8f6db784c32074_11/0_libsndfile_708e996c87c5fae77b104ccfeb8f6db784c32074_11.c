 header_put_marker (SF_PRIVATE *psf, int x)
{	psf->header.ptr [psf->header.indx++] = (x >> 24) ;
	psf->header.ptr [psf->header.indx++] = (x >> 16) ;
	psf->header.ptr [psf->header.indx++] = (x >> 8) ;
	psf->header.ptr [psf->header.indx++] = x ;
 } /* header_put_marker */
