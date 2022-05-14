 header_gets (SF_PRIVATE *psf, char *ptr, int bufsize)
 {	int		k ;
 
 	for (k = 0 ; k < bufsize - 1 ; k++)
	{	if (psf->headindex < psf->headend)
		{	ptr [k] = psf->header [psf->headindex] ;
			psf->headindex ++ ;
 			}
 		else
		{	psf->headend += psf_fread (psf->header + psf->headend, 1, 1, psf) ;
			ptr [k] = psf->header [psf->headindex] ;
			psf->headindex = psf->headend ;
 			} ;
 
 		if (ptr [k] == '\n')
			break ;
		} ;

	ptr [k] = 0 ;

	return k ;
} /* header_gets */
