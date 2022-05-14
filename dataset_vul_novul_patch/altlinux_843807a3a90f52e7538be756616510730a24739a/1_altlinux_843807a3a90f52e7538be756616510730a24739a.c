check_acl(pam_handle_t *pamh,
	  const char *sense, const char *this_user, const char *other_user,
	  int noent_code, int debug)
{
        struct passwd *pwd;
        FILE *fp = NULL;
        int i, fd = -1, save_errno;
       uid_t fsuid;
        struct stat st;
        /* Check this user's <sense> file. */
        pwd = pam_modutil_getpwnam(pamh, this_user);
        if (pwd == NULL) {
		pam_syslog(pamh, LOG_ERR,
			   "error determining home directory for '%s'",
			   this_user);
		return PAM_SESSION_ERR;
	}
	/* Figure out what that file is really named. */
	i = snprintf(path, sizeof(path), "%s/.xauth/%s", pwd->pw_dir, sense);
	if ((i >= (int)sizeof(path)) || (i < 0)) {
		pam_syslog(pamh, LOG_ERR,
                           "name of user's home directory is too long");
                return PAM_SESSION_ERR;
        }
       fsuid = setfsuid(pwd->pw_uid);
        if (!stat(path, &st)) {
                if (!S_ISREG(st.st_mode))
                        errno = EINVAL;
			fd = open(path, O_RDONLY | O_NOCTTY);
                        fd = open(path, O_RDONLY | O_NOCTTY);
        }
        save_errno = errno;
       setfsuid(fsuid);
        if (fd >= 0) {
                if (!fstat(fd, &st)) {
                        if (!S_ISREG(st.st_mode))
			save_errno = errno;
			close(fd);
		}
	}
	if (fp) {
		char buf[LINE_MAX], *tmp;
		/* Scan the file for a list of specs of users to "trust". */
		while (fgets(buf, sizeof(buf), fp) != NULL) {
			tmp = memchr(buf, '\r', sizeof(buf));
			if (tmp != NULL) {
				*tmp = '\0';
			}
			tmp = memchr(buf, '\n', sizeof(buf));
			if (tmp != NULL) {
				*tmp = '\0';
			}
			if (fnmatch(buf, other_user, 0) == 0) {
				if (debug) {
					pam_syslog(pamh, LOG_DEBUG,
						   "%s %s allowed by %s",
						   other_user, sense, path);
				}
				fclose(fp);
				return PAM_SUCCESS;
			}
		}
		/* If there's no match in the file, we fail. */
		if (debug) {
			pam_syslog(pamh, LOG_DEBUG, "%s not listed in %s",
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
