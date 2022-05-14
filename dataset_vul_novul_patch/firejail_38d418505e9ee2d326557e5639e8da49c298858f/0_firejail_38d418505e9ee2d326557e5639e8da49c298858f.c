static void skel(const char *homedir, uid_t u, gid_t g) {
	char *fname;

	if (!arg_shell_none && (strcmp(cfg.shell,"/usr/bin/zsh") == 0 || strcmp(cfg.shell,"/bin/zsh") == 0)) {
		if (asprintf(&fname, "%s/.zshrc", homedir) == -1)
			errExit("asprintf");
		struct stat s;
 		if (stat(fname, &s) == 0)
 			return;
		if (is_link(fname)) { // stat on dangling symlinks fails, try again using lstat
			fprintf(stderr, "Error: invalid %s file\n", fname);
			exit(1);
		}
 		if (stat("/etc/skel/.zshrc", &s) == 0) {
			copy_file_as_user("/etc/skel/.zshrc", fname, u, g, 0644);
 			fs_logger("clone /etc/skel/.zshrc");
 		}
 		else {
			touch_file_as_user(fname, u, g, 0644);
			fs_logger2("touch", fname);
		}
		free(fname);
	}
	else if (!arg_shell_none && strcmp(cfg.shell,"/bin/csh") == 0) {
		if (asprintf(&fname, "%s/.cshrc", homedir) == -1)
			errExit("asprintf");
		struct stat s;
 		if (stat(fname, &s) == 0)
 			return;
		if (is_link(fname)) { // stat on dangling symlinks fails, try again using lstat
			fprintf(stderr, "Error: invalid %s file\n", fname);
			exit(1);
		}
 		if (stat("/etc/skel/.cshrc", &s) == 0) {
			copy_file_as_user("/etc/skel/.cshrc", fname, u, g, 0644);
 			fs_logger("clone /etc/skel/.cshrc");
 		}
 		else {
			touch_file_as_user(fname, u, g, 0644);
			fs_logger2("touch", fname);
		}
		free(fname);
	}
	else {
		if (asprintf(&fname, "%s/.bashrc", homedir) == -1)
			errExit("asprintf");
		struct stat s;
 		if (stat(fname, &s) == 0) 
 			return;
		if (is_link(fname)) { // stat on dangling symlinks fails, try again using lstat
			fprintf(stderr, "Error: invalid %s file\n", fname);
			exit(1);
		}
 		if (stat("/etc/skel/.bashrc", &s) == 0) {
			copy_file_as_user("/etc/skel/.bashrc", fname, u, g, 0644);
 			fs_logger("clone /etc/skel/.bashrc");
 		}
 		free(fname);
	}
}
