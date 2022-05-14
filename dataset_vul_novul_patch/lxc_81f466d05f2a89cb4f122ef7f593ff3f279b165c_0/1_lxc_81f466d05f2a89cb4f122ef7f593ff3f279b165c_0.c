int lsm_set_label_at(int procfd, int on_exec, char* lsm_label) {
 	int labelfd = -1;
	int ret = 0;
 	const char* name;
	char* command = NULL;
 
 	name = lsm_name();
 
 	if (strcmp(name, "nop") == 0)
		goto out;
 
 	if (strcmp(name, "none") == 0)
		goto out;
 
 	/* We don't support on-exec with AppArmor */
 	if (strcmp(name, "AppArmor") == 0)
 		on_exec = 0;
 
 	if (on_exec) {
		labelfd = openat(procfd, "self/attr/exec", O_RDWR);
	}
	else {
		labelfd = openat(procfd, "self/attr/current", O_RDWR);
 	}
 
 	if (labelfd < 0) {
 		SYSERROR("Unable to open LSM label");
		ret = -1;
		goto out;
 	}
 
 	if (strcmp(name, "AppArmor") == 0) {
 		int size;
 
 		command = malloc(strlen(lsm_label) + strlen("changeprofile ") + 1);
 		if (!command) {
 			SYSERROR("Failed to write apparmor profile");
			ret = -1;
 			goto out;
 		}
 
 		size = sprintf(command, "changeprofile %s", lsm_label);
 		if (size < 0) {
 			SYSERROR("Failed to write apparmor profile");
			ret = -1;
 			goto out;
 		}
 
		if (write(labelfd, command, size + 1) < 0) {
			SYSERROR("Unable to set LSM label");
			ret = -1;
 			goto out;
 		}
	}
	else if (strcmp(name, "SELinux") == 0) {
		if (write(labelfd, lsm_label, strlen(lsm_label) + 1) < 0) {
 			SYSERROR("Unable to set LSM label");
			ret = -1;
 			goto out;
 		}
	}
	else {
 		ERROR("Unable to restore label for unknown LSM: %s", name);
		ret = -1;
 		goto out;
 	}
 
 out:
 	free(command);
 
	if (labelfd != -1)
		close(labelfd);
 
	return ret;
 }
