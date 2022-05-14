 header_put_be_8byte (SF_PRIVATE *psf, sf_count_t x)
{	psf->header.ptr [psf->header.indx++] = (x >> 56) ;
	psf->header.ptr [psf->header.indx++] = (x >> 48) ;
	psf->header.ptr [psf->header.indx++] = (x >> 40) ;
	psf->header.ptr [psf->header.indx++] = (x >> 32) ;
	psf->header.ptr [psf->header.indx++] = (x >> 24) ;
	psf->header.ptr [psf->header.indx++] = (x >> 16) ;
	psf->header.ptr [psf->header.indx++] = (x >> 8) ;
	psf->header.ptr [psf->header.indx++] = x ;
 } /* header_put_be_8byte */
