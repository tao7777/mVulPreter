psf_asciiheader_printf (SF_PRIVATE *psf, const char *format, ...)
{	va_list	argptr ;
 	int		maxlen ;
 	char	*start ;
 
	maxlen = strlen ((char*) psf->header.ptr) ;
	start	= ((char*) psf->header.ptr) + maxlen ;
	maxlen	= psf->header.len - maxlen ;
 
 	va_start (argptr, format) ;
 	vsnprintf (start, maxlen, format, argptr) ;
	va_end (argptr) ;

 	/* Make sure the string is properly terminated. */
 	start [maxlen - 1] = 0 ;
 
	psf->header.indx = strlen ((char*) psf->header.ptr) ;
 
 	return ;
 } /* psf_asciiheader_printf */
