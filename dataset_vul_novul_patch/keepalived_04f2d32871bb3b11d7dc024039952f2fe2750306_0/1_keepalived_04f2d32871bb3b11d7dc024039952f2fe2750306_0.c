 pidfile_write(const char *pid_file, int pid)
 {
 	FILE *pidfile = NULL;
	int pidfd = creat(pid_file, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
 
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
