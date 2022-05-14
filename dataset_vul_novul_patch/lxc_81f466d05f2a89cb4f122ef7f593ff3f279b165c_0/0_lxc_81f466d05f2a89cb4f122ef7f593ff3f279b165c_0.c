int lsm_set_label_at(int procfd, int on_exec, char* lsm_label) {
static int lsm_openat(int procfd, pid_t pid, int on_exec)
{
	int ret = -1;
 	int labelfd = -1;
 	const char* name;
#define __LSMATTRLEN /* /proc */ (5 + /* /pid-to-str */ 21 + /* /current */ 7 + /* \0 */ 1)
	char path[__LSMATTRLEN];
 
 	name = lsm_name();
 
 	if (strcmp(name, "nop") == 0)
		return 0;
 
 	if (strcmp(name, "none") == 0)
		return 0;
 
 	/* We don't support on-exec with AppArmor */
 	if (strcmp(name, "AppArmor") == 0)
 		on_exec = 0;
 
 	if (on_exec) {
		ret = snprintf(path, __LSMATTRLEN, "%d/attr/exec", pid);
		if (ret < 0 || ret >= __LSMATTRLEN)
			return -1;
		labelfd = openat(procfd, path, O_RDWR);
	} else {
		ret = snprintf(path, __LSMATTRLEN, "%d/attr/current", pid);
		if (ret < 0 || ret >= __LSMATTRLEN)
			return -1;
		labelfd = openat(procfd, path, O_RDWR);
 	}
 
 	if (labelfd < 0) {
 		SYSERROR("Unable to open LSM label");
		return -1;
 	}
 
	return labelfd;
}

static int lsm_set_label_at(int lsm_labelfd, int on_exec, char *lsm_label)
{
	int fret = -1;
	const char* name;
	char *command = NULL;

	name = lsm_name();

	if (strcmp(name, "nop") == 0)
		return 0;

	if (strcmp(name, "none") == 0)
		return 0;

	/* We don't support on-exec with AppArmor */
	if (strcmp(name, "AppArmor") == 0)
		on_exec = 0;

 	if (strcmp(name, "AppArmor") == 0) {
 		int size;
 
 		command = malloc(strlen(lsm_label) + strlen("changeprofile ") + 1);
 		if (!command) {
 			SYSERROR("Failed to write apparmor profile");
 			goto out;
 		}
 
 		size = sprintf(command, "changeprofile %s", lsm_label);
 		if (size < 0) {
 			SYSERROR("Failed to write apparmor profile");
 			goto out;
 		}
 
		if (write(lsm_labelfd, command, size + 1) < 0) {
			SYSERROR("Unable to set LSM label: %s.", command);
 			goto out;
 		}
		INFO("Set LSM label to: %s.", command);
	} else if (strcmp(name, "SELinux") == 0) {
		if (write(lsm_labelfd, lsm_label, strlen(lsm_label) + 1) < 0) {
 			SYSERROR("Unable to set LSM label");
 			goto out;
 		}
		INFO("Set LSM label to: %s.", lsm_label);
	} else {
 		ERROR("Unable to restore label for unknown LSM: %s", name);
 		goto out;
 	}
	fret = 0;
 
 out:
 	free(command);
 
	if (lsm_labelfd != -1)
		close(lsm_labelfd);
 
	return fret;
 }
