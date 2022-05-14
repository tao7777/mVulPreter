check_acl(pam_handle_t *pamh,
	  const char *sense, const char *this_user, const char *other_user,
	  int noent_code, int debug)
{
	char path[PATH_MAX];
	struct passwd *pwd;
 {
        char path[PATH_MAX];
        struct passwd *pwd;
       FILE *fp;
       int i, save_errno;
        uid_t fsuid;
        /* Check this user's <sense> file. */
        pwd = pam_modutil_getpwnam(pamh, this_user);
        if (pwd == NULL) {
	}
	/* Figure out what that file is really named. */
	i = snprintf(path, sizeof(path), "%s/.xauth/%s", pwd->pw_dir, sense);
	if ((i >= (int)sizeof(path)) || (i < 0)) {
		pam_syslog(pamh, LOG_ERR,
			   "name of user's home directory is too long");
		return PAM_SESSION_ERR;
	}
	fsuid = setfsuid(pwd->pw_uid);
	fp = fopen(path, "r");
                return PAM_SESSION_ERR;
        }
        fsuid = setfsuid(pwd->pw_uid);
       fp = fopen(path, "r");
        save_errno = errno;
        setfsuid(fsuid);
       if (fp != NULL) {
                char buf[LINE_MAX], *tmp;
                /* Scan the file for a list of specs of users to "trust". */
                while (fgets(buf, sizeof(buf), fp) != NULL) {
				   other_user, path);
		}
		fclose(fp);
		return PAM_PERM_DENIED;
	} else {
		/* Default to okay if the file doesn't exist. */
	        errno = save_errno;
		switch (errno) {
		case ENOENT:
			if (noent_code == PAM_SUCCESS) {
				if (debug) {
					pam_syslog(pamh, LOG_DEBUG,
						   "%s does not exist, ignoring",
						   path);
				}
			} else {
				if (debug) {
					pam_syslog(pamh, LOG_DEBUG,
						   "%s does not exist, failing",
						   path);
				}
			}
			return noent_code;
		default:
			if (debug) {
				pam_syslog(pamh, LOG_DEBUG,
					   "error opening %s: %m", path);
			}
			return PAM_PERM_DENIED;
		}
	}
}
