 header_read (SF_PRIVATE *psf, void *ptr, int bytes)
 {	int count = 0 ;
 
	if (psf->header.indx + bytes >= psf->header.len && psf_bump_header_allocation (psf, bytes))
		return count ;
 
	if (psf->header.indx + bytes > psf->header.end)
	{	count = psf_fread (psf->header.ptr + psf->header.end, 1, bytes - (psf->header.end - psf->header.indx), psf) ;
		if (count != bytes - (int) (psf->header.end - psf->header.indx))
 		{	psf_log_printf (psf, "Error : psf_fread returned short count.\n") ;
 			return count ;
 			} ;
		psf->header.end += count ;
 		} ;
 
	memcpy (ptr, psf->header.ptr + psf->header.indx, bytes) ;
	psf->header.indx += bytes ;
 
 	return bytes ;
 } /* header_read */
