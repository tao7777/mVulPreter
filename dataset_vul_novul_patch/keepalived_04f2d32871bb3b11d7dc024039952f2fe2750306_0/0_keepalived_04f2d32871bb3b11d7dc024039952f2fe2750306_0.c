 pidfile_write(const char *pid_file, int pid)
 {
 	FILE *pidfile = NULL;
	int pidfd = open(pid_file, O_NOFOLLOW | O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
 
 	if (pidfd != -1) pidfile = fdopen(pidfd, "w");
 
	if (!pidfile) {
		log_message(LOG_INFO, "pidfile_write : Cannot open %s pidfile",
		       pid_file);
		return 0;
	}
	fprintf(pidfile, "%d\n", pid);
	fclose(pidfile);
	return 1;
}
