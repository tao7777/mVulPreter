 static FILE *open_log_file(void)
 {
	int fh;
	struct stat st;

 	if(log_fp) /* keep it open unless we rotate */
 		return log_fp;
 
	if ((fh = open(log_file, O_RDWR|O_APPEND|O_CREAT|O_NOFOLLOW, S_IRUSR|S_IWUSR)) == -1) {
		if (daemon_mode == FALSE)
			printf("Warning: Cannot open log file '%s' for writing\n", log_file);
		return NULL;
	}
	log_fp = fdopen(fh, "a+");
 	if(log_fp == NULL) {
		if (daemon_mode == FALSE)
 			printf("Warning: Cannot open log file '%s' for writing\n", log_file);
 		return NULL;
 		}
 
	if ((fstat(fh, &st)) == -1) {
		log_fp = NULL;
		close(fh);
		if (daemon_mode == FALSE)
			printf("Warning: Cannot fstat log file '%s'\n", log_file);
		return NULL;
	}
	if (st.st_nlink != 1 || (st.st_mode & S_IFMT) != S_IFREG) {
		log_fp = NULL;
		close(fh);
		if (daemon_mode == FALSE)
			printf("Warning: log file '%s' has an invalid mode\n", log_file);
		return NULL;
	}

 	(void)fcntl(fileno(log_fp), F_SETFD, FD_CLOEXEC);
 	return log_fp;
 }
