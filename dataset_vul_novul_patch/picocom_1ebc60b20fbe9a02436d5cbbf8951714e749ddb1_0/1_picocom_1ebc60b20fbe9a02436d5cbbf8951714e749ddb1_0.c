run_cmd(int fd, ...)
 {
 	pid_t pid;
 	sigset_t sigm, sigm_old;

	/* block signals, let child establish its own handlers */
	sigemptyset(&sigm);
	sigaddset(&sigm, SIGTERM);
	sigprocmask(SIG_BLOCK, &sigm, &sigm_old);

	pid = fork();
	if ( pid < 0 ) {
		sigprocmask(SIG_SETMASK, &sigm_old, NULL);
		fd_printf(STO, "*** cannot fork: %s ***\r\n", strerror(errno));
		return -1;
	} else if ( pid ) {
		/* father: picocom */
		int status, r;

		/* reset the mask */
		sigprocmask(SIG_SETMASK, &sigm_old, NULL);
		/* wait for child to finish */
		do {
			r = waitpid(pid, &status, 0);
		} while ( r < 0 && errno == EINTR );
		/* reset terminal (back to raw mode) */
		term_apply(STI);
		/* check and report child return status */
		if ( WIFEXITED(status) ) { 
			fd_printf(STO, "\r\n*** exit status: %d ***\r\n", 
					  WEXITSTATUS(status));
			return WEXITSTATUS(status);
		} else if ( WIFSIGNALED(status) ) {
			fd_printf(STO, "\r\n*** killed by signal: %d ***\r\n", 
					  WTERMSIG(status));
			return -1;
		} else {
			fd_printf(STO, "\r\n*** abnormal termination: 0x%x ***\r\n", r);
			return -1;
		}
 	} else {
 		/* child: external program */
 		long fl;
		char cmd[512];
 		/* unmanage terminal, and reset it to canonical mode */
 		term_remove(STI);
 		/* unmanage serial port fd, without reset */
		term_erase(fd);
		/* set serial port fd to blocking mode */
		fl = fcntl(fd, F_GETFL); 
		fl &= ~O_NONBLOCK;
		fcntl(fd, F_SETFL, fl);
		/* connect stdin and stdout to serial port */
		close(STI);
 		close(STO);
 		dup2(fd, STI);
 		dup2(fd, STO);
		{
			/* build command-line */
			char *c, *ce;
			const char *s;
			int n;
			va_list vls;
			strcpy(cmd, EXEC);
			c = &cmd[sizeof(EXEC)- 1];
			ce = cmd + sizeof(cmd) - 1;
			va_start(vls, fd);
			while ( (s = va_arg(vls, const char *)) ) {
				n = strlen(s);
				if ( c + n + 1 >= ce ) break;
				memcpy(c, s, n); c += n;
				*c++ = ' ';
			}
			va_end(vls);
			*c = '\0';
 		}
 		/* run extenral command */
		fd_printf(STDERR_FILENO, "%s\n", &cmd[sizeof(EXEC) - 1]);
 		establish_child_signal_handlers();
 		sigprocmask(SIG_SETMASK, &sigm_old, NULL);
		execl("/bin/sh", "sh", "-c", cmd, NULL);
		exit(42);
 	}
 }
