 static FILE *open_log_file(void)
 {
 	if(log_fp) /* keep it open unless we rotate */
 		return log_fp;
 
	log_fp = fopen(log_file, "a+");
 	if(log_fp == NULL) {
		if (daemon_mode == FALSE) {
 			printf("Warning: Cannot open log file '%s' for writing\n", log_file);
			}
 		return NULL;
 		}
 
 	(void)fcntl(fileno(log_fp), F_SETFD, FD_CLOEXEC);
 	return log_fp;
 }
