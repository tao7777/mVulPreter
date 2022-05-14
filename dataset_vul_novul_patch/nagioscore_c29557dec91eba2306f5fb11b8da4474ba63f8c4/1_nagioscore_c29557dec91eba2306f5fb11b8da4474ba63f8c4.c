int open_debug_log(void) {
 
 	/* don't do anything if we're not actually running... */
 	if(verify_config || test_scheduling == TRUE)
		return OK;

	/* don't do anything if we're not debugging */
 	if(debug_level == DEBUGL_NONE)
 		return OK;
 
	if((debug_file_fp = fopen(debug_file, "a+")) == NULL)
 		return ERROR;
 
	(void)fcntl(fileno(debug_file_fp), F_SETFD, FD_CLOEXEC);
 
 	return OK;
 	}
