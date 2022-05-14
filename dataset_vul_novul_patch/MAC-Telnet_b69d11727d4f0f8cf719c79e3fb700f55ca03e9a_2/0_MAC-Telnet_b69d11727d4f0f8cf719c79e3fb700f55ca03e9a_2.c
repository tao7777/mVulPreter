static void user_login(struct mt_connection *curconn, struct mt_mactelnet_hdr *pkthdr) {
	struct mt_packet pdata;
	unsigned char md5sum[17];
 	char md5data[100];
 	struct mt_credentials *user;
 	char *slavename;
	int act_pass_len;
 
 	/* Reparse user file before each login */
 	read_userfile();

	if ((user = find_user(curconn->username)) != NULL) {
		md5_state_t state;
#if defined(__linux__) && defined(_POSIX_MEMLOCK_RANGE)
		mlock(md5data, sizeof(md5data));
		mlock(md5sum, sizeof(md5sum));
		if (user->password != NULL) {
			mlock(user->password, strlen(user->password));
 		}
 #endif
 
		/* calculate the password's actual length */
		act_pass_len = strlen(user->password);
		act_pass_len = act_pass_len <= 82 ? act_pass_len : 82;

 		/* Concat string of 0 + password + pass_salt */
 		md5data[0] = 0;
		memcpy(md5data + 1, user->password, act_pass_len);
		memcpy(md5data + 1 + act_pass_len, curconn->pass_salt, 16);
 
 		/* Generate md5 sum of md5data with a leading 0 */
 		md5_init(&state);
		md5_append(&state, (const md5_byte_t *)md5data, 1 + act_pass_len + 16);
 		md5_finish(&state, (md5_byte_t *)md5sum + 1);
 		md5sum[0] = 0;
 
		init_packet(&pdata, MT_PTYPE_DATA, pkthdr->dstaddr, pkthdr->srcaddr, pkthdr->seskey, curconn->outcounter);
		curconn->outcounter += add_control_packet(&pdata, MT_CPTYPE_END_AUTH, NULL, 0);
		send_udp(curconn, &pdata);

		if (curconn->state == STATE_ACTIVE) {
			return;
		}
	}

	if (user == NULL || memcmp(md5sum, curconn->trypassword, 17) != 0) {
		syslog(LOG_NOTICE, _("(%d) Invalid login by %s."), curconn->seskey, curconn->username);

		/*_ Please include both \r and \n in translation, this is needed for the terminal emulator. */
		abort_connection(curconn, pkthdr, _("Login failed, incorrect username or password\r\n"));

		/* TODO: should wait some time (not with sleep) before returning, to minimalize brute force attacks */
		return;
	}

	/* User is logged in */
	curconn->state = STATE_ACTIVE;

	/* Enter terminal mode */
	curconn->terminal_mode = 1;

	/* Open pts handle */
	curconn->ptsfd = posix_openpt(O_RDWR);
	if (curconn->ptsfd == -1 || grantpt(curconn->ptsfd) == -1 || unlockpt(curconn->ptsfd) == -1) {
			syslog(LOG_ERR, "posix_openpt: %s", strerror(errno));
			/*_ Please include both \r and \n in translation, this is needed for the terminal emulator. */
			abort_connection(curconn, pkthdr, _("Terminal error\r\n"));
			return;
	}

	/* Get file path for our pts */
	slavename = ptsname(curconn->ptsfd);
	if (slavename != NULL) {
		pid_t pid;
		struct stat sb;
		struct passwd *user = (struct passwd *)malloc(sizeof(struct passwd));
		struct passwd *tmpuser=user;
		char *buffer = malloc(1024);

		if (user == NULL || buffer == NULL) {
			syslog(LOG_CRIT, _("(%d) Error allocating memory."), curconn->seskey);
			/*_ Please include both \r and \n in translation, this is needed for the terminal emulator. */
			abort_connection(curconn, pkthdr, _("System error, out of memory\r\n"));
			return;
		}

		if (getpwnam_r(curconn->username, user, buffer, 1024, &tmpuser) != 0) {
			syslog(LOG_WARNING, _("(%d) Login ok, but local user not accessible (%s)."), curconn->seskey, curconn->username);
			/*_ Please include both \r and \n in translation, this is needed for the terminal emulator. */
			abort_connection(curconn, pkthdr, _("Local user not accessible\r\n"));
			free(user);
			free(buffer);
			return;
		}

		/* Change the owner of the slave pts */
		chown(slavename, user->pw_uid, user->pw_gid);

		curconn->slavefd = open(slavename, O_RDWR);
		if (curconn->slavefd == -1) {
			syslog(LOG_ERR, _("Error opening %s: %s"), slavename, strerror(errno));
			/*_ Please include both \r and \n in translation, this is needed for the terminal emulator. */
			abort_connection(curconn, pkthdr, _("Error opening terminal\r\n"));
			list_remove_connection(curconn);
			return;
		}

		if ((pid = fork()) == 0) {
			struct net_interface *interface;

			/* Add login information to utmp/wtmp */
			uwtmp_login(curconn);

			syslog(LOG_INFO, _("(%d) User %s logged in."), curconn->seskey, curconn->username);

			/* Initialize terminal environment */
			setenv("USER", user->pw_name, 1);
			setenv("HOME", user->pw_dir, 1);
			setenv("SHELL", user->pw_shell, 1);
			setenv("TERM", curconn->terminal_type, 1);
			close(sockfd);
			close(insockfd);

			DL_FOREACH(interfaces, interface) {
				if (interface->socketfd > 0) {
					close(interface->socketfd);
				}
			}
			setsid();

			/* Don't let shell process inherit slavefd */
			fcntl (curconn->slavefd, F_SETFD, FD_CLOEXEC);
			close(curconn->ptsfd);

			/* Redirect STDIN/STDIO/STDERR */
			close(0);
			dup(curconn->slavefd);
			close(1);
			dup(curconn->slavefd);
			close(2);
			dup(curconn->slavefd);

			/* Set controlling terminal */
			ioctl(0, TIOCSCTTY, 1);
			tcsetpgrp(0, getpid());

			/* Set user id/group id */
			if ((setgid(user->pw_gid) != 0) || (setuid(user->pw_uid) != 0)) {
				syslog(LOG_ERR, _("(%d) Could not log in %s (%d:%d): setuid/setgid: %s"), curconn->seskey, curconn->username, user->pw_uid, user->pw_gid, strerror(errno));
				/*_ Please include both \r and \n in translation, this is needed for the terminal emulator. */
				abort_connection(curconn, pkthdr, _("Internal error\r\n"));
				exit(0);
			}

			/* Abort login if /etc/nologin exists */
			if (stat(_PATH_NOLOGIN, &sb) == 0 && getuid() != 0) {
				syslog(LOG_NOTICE, _("(%d) User %s disconnected with " _PATH_NOLOGIN " message."), curconn->seskey, curconn->username);
				display_nologin();
				curconn->state = STATE_CLOSED;
				init_packet(&pdata, MT_PTYPE_END, pkthdr->dstaddr, pkthdr->srcaddr, pkthdr->seskey, curconn->outcounter);
				send_udp(curconn, &pdata);
				exit(0);
			}

			/* Display MOTD */
			display_motd();

			chdir(user->pw_dir);

			/* Spawn shell */
			/* TODO: Maybe use "login -f USER" instead? renders motd and executes shell correctly for system */
			execl(user->pw_shell, user->pw_shell, "-", (char *) 0);
			exit(0); // just to be sure.
		}
		free(user);
		free(buffer);
		close(curconn->slavefd);
		curconn->pid = pid;
		set_terminal_size(curconn->ptsfd, curconn->terminal_width, curconn->terminal_height);
	}

}
