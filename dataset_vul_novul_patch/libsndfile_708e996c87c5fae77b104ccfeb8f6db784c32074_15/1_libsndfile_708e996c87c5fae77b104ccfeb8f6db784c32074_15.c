psf_asciiheader_printf (SF_PRIVATE *psf, const char *format, ...)
{	va_list	argptr ;
 	int		maxlen ;
 	char	*start ;
 
	maxlen = strlen ((char*) psf->header) ;
	start	= ((char*) psf->header) + maxlen ;
	maxlen	= sizeof (psf->header) - maxlen ;
 
 	va_start (argptr, format) ;
 	vsnprintf (start, maxlen, format, argptr) ;
	va_end (argptr) ;

 	/* Make sure the string is properly terminated. */
 	start [maxlen - 1] = 0 ;
 
	psf->headindex = strlen ((char*) psf->header) ;
 
 	return ;
 } /* psf_asciiheader_printf */
