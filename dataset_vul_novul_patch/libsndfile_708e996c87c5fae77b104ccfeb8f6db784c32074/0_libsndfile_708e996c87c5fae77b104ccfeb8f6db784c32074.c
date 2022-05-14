 header_gets (SF_PRIVATE *psf, char *ptr, int bufsize)
 {	int		k ;
 
	if (psf->header.indx + bufsize >= psf->header.len && psf_bump_header_allocation (psf, bufsize))
		return 0 ;

 	for (k = 0 ; k < bufsize - 1 ; k++)
	{	if (psf->header.indx < psf->header.end)
		{	ptr [k] = psf->header.ptr [psf->header.indx] ;
			psf->header.indx ++ ;
 			}
 		else
		{	psf->header.end += psf_fread (psf->header.ptr + psf->header.end, 1, 1, psf) ;
			ptr [k] = psf->header.ptr [psf->header.indx] ;
			psf->header.indx = psf->header.end ;
 			} ;
 
 		if (ptr [k] == '\n')
			break ;
		} ;

	ptr [k] = 0 ;

	return k ;
} /* header_gets */
