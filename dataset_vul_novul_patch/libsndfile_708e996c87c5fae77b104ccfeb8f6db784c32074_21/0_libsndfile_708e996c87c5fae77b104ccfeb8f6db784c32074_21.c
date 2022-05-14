sf_open_virtual	(SF_VIRTUAL_IO *sfvirtual, int mode, SF_INFO *sfinfo, void *user_data)
{	SF_PRIVATE 	*psf ;

	/* Make sure we have a valid set ot virtual pointers. */
	if (sfvirtual->get_filelen == NULL || sfvirtual->seek == NULL || sfvirtual->tell == NULL)
	{	sf_errno = SFE_BAD_VIRTUAL_IO ;
		snprintf (sf_parselog, sizeof (sf_parselog), "Bad vio_get_filelen / vio_seek / vio_tell in SF_VIRTUAL_IO struct.\n") ;
		return NULL ;
		} ;

	if ((mode == SFM_READ || mode == SFM_RDWR) && sfvirtual->read == NULL)
	{	sf_errno = SFE_BAD_VIRTUAL_IO ;
		snprintf (sf_parselog, sizeof (sf_parselog), "Bad vio_read in SF_VIRTUAL_IO struct.\n") ;
		return NULL ;
		} ;

	if ((mode == SFM_WRITE || mode == SFM_RDWR) && sfvirtual->write == NULL)
	{	sf_errno = SFE_BAD_VIRTUAL_IO ;
		snprintf (sf_parselog, sizeof (sf_parselog), "Bad vio_write in SF_VIRTUAL_IO struct.\n") ;
 		return NULL ;
 		} ;
 
	if ((psf = psf_allocate ()) == NULL)
 	{	sf_errno = SFE_MALLOC_FAILED ;
 		return	NULL ;
 		} ;

	psf_init_files (psf) ;

	psf->virtual_io = SF_TRUE ;
	psf->vio = *sfvirtual ;
	psf->vio_user_data = user_data ;

	psf->file.mode = mode ;

	return psf_open_file (psf, sfinfo) ;
} /* sf_open_virtual */
