sf_open	(const char *path, int mode, SF_INFO *sfinfo)
{	SF_PRIVATE 	*psf ;

 	/* Ultimate sanity check. */
 	assert (sizeof (sf_count_t) == 8) ;
 
	if ((psf = psf_allocate ()) == NULL)
 	{	sf_errno = SFE_MALLOC_FAILED ;
 		return	NULL ;
 		} ;

	psf_init_files (psf) ;

	psf_log_printf (psf, "File : %s\n", path) ;

	if (copy_filename (psf, path) != 0)
	{	sf_errno = psf->error ;
		return	NULL ;
		} ;

	psf->file.mode = mode ;
	if (strcmp (path, "-") == 0)
		psf->error = psf_set_stdio (psf) ;
	else
		psf->error = psf_fopen (psf) ;

	return psf_open_file (psf, sfinfo) ;
} /* sf_open */
