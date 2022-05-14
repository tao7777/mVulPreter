static void copy_xauthority(void) {
	char *src = RUN_XAUTHORITY_FILE ;
	char *dest;
	if (asprintf(&dest, "%s/.Xauthority", cfg.homedir) == -1)
		errExit("asprintf");
	
	if (is_link(dest)) {
		fprintf(stderr, "Error: %s is a symbolic link\n", dest);
 		exit(1);
 	}
 
	copy_file_as_user(src, dest, getuid(), getgid(), S_IRUSR | S_IWUSR);
	fs_logger2("clone", dest);
 	
 	unlink(src);
}
