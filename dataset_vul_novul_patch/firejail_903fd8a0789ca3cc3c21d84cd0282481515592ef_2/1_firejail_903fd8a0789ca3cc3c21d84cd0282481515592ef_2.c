static int store_asoundrc(void) {
	fs_build_mnt_dir();
 
 	char *src;
 	char *dest = RUN_ASOUNDRC_FILE;
 	FILE *fp = fopen(dest, "w");
 	if (fp) {
 		fprintf(fp, "\n");
 		SET_PERMS_STREAM(fp, getuid(), getgid(), 0644);
 		fclose(fp);
 	}
 	if (asprintf(&src, "%s/.asoundrc", cfg.homedir) == -1)
 		errExit("asprintf");
 	
 	struct stat s;
	if (stat(src, &s) == 0) {	
 		if (is_link(src)) {
			fprintf(stderr, "Error: invalid .asoundrc file\n");
			exit(1);
		}
		pid_t child = fork();
		if (child < 0)
			errExit("fork");
		if (child == 0) {
			drop_privs(0);
			int rv = copy_file(src, dest);
			if (rv)
				fprintf(stderr, "Warning: cannot transfer .asoundrc in private home directory\n");
			else {
				fs_logger2("clone", dest);
 			}
			_exit(0);
 		}
		waitpid(child, NULL, 0);
 
		if (chown(dest, getuid(), getgid()) == -1)
			errExit("fchown");
		if (chmod(dest, 0644) == -1)
			errExit("fchmod");
 		return 1; // file copied
 	}
 	
	return 0;
}
