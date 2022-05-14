static void copy_asoundrc(void) {
	char *src = RUN_ASOUNDRC_FILE ;
 	char *dest;
 	if (asprintf(&dest, "%s/.asoundrc", cfg.homedir) == -1)
 		errExit("asprintf");
	
 	if (is_link(dest)) {
 		fprintf(stderr, "Error: %s is a symbolic link\n", dest);
 		exit(1);
 	}
 
	copy_file_as_user(src, dest, getuid(), getgid(), S_IRUSR | S_IWUSR); // regular user
	fs_logger2("clone", dest);
 
 	unlink(src);
}
