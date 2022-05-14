 header_read (SF_PRIVATE *psf, void *ptr, int bytes)
 {	int count = 0 ;
 
	if (psf->headindex >= SIGNED_SIZEOF (psf->header))
		return psf_fread (ptr, 1, bytes, psf) ;
	if (psf->headindex + bytes > SIGNED_SIZEOF (psf->header))
	{	int most ;
 
		most = SIGNED_SIZEOF (psf->header) - psf->headend ;
		psf_fread (psf->header + psf->headend, 1, most, psf) ;
		memcpy (ptr, psf->header + psf->headend, most) ;
		psf->headend = psf->headindex += most ;
		psf_fread ((char *) ptr + most, bytes - most, 1, psf) ;
		return bytes ;
		} ;
	if (psf->headindex + bytes > psf->headend)
	{	count = psf_fread (psf->header + psf->headend, 1, bytes - (psf->headend - psf->headindex), psf) ;
		if (count != bytes - (int) (psf->headend - psf->headindex))
 		{	psf_log_printf (psf, "Error : psf_fread returned short count.\n") ;
 			return count ;
 			} ;
		psf->headend += count ;
 		} ;
 
	memcpy (ptr, psf->header + psf->headindex, bytes) ;
	psf->headindex += bytes ;
 
 	return bytes ;
 } /* header_read */
