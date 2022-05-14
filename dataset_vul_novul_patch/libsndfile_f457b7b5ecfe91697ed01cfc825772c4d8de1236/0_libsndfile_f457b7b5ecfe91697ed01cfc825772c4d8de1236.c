id3_skip (SF_PRIVATE * psf)
{	unsigned char	buf [10] ;

	memset (buf, 0, sizeof (buf)) ;
	psf_binheader_readf (psf, "pb", 0, buf, 10) ;

	if (buf [0] == 'I' && buf [1] == 'D' && buf [2] == '3')
	{	int	offset = buf [6] & 0x7f ;
		offset = (offset << 7) | (buf [7] & 0x7f) ;
		offset = (offset << 7) | (buf [8] & 0x7f) ;
		offset = (offset << 7) | (buf [9] & 0x7f) ;

		psf_log_printf (psf, "ID3 length : %d\n--------------------\n", offset) ;

		/* Never want to jump backwards in a file. */
		if (offset < 0)
			return 0 ;
 
 		/* Calculate new file offset and position ourselves there. */
 		psf->fileoffset += offset + 10 ;
 
		if (psf->fileoffset < psf->filelength)
		{	psf_binheader_readf (psf, "p", psf->fileoffset) ;
			return 1 ;
			} ;
 		} ;
 
 	return 0 ;
} /* id3_skip */
