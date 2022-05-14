static void copy_xauthority(void) {
	char *src = RUN_XAUTHORITY_FILE ;
	char *dest;
	if (asprintf(&dest, "%s/.Xauthority", cfg.homedir) == -1)
		errExit("asprintf");
	
	if (is_link(dest)) {
		fprintf(stderr, "Error: %s is a symbolic link\n", dest);
 		exit(1);
 	}
 
	pid_t child = fork();
	if (child < 0)
		errExit("fork");
	if (child == 0) {
		drop_privs(0);
		int rv = copy_file(src, dest, getuid(), getgid(), S_IRUSR | S_IWUSR);
		if (rv)
			fprintf(stderr, "Warning: cannot transfer .Xauthority in private home directory\n");
		else {
			fs_logger2("clone", dest);
		}
		_exit(0);
	}
	waitpid(child, NULL, 0);
 	
 	unlink(src);
}
