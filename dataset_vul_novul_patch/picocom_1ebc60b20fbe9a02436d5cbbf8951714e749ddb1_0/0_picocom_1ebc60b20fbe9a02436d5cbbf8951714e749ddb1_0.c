run_cmd(int fd, ...)
run_cmd(int fd, const char *cmd, const char *args_extra)
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
		int argc;
		char *argv[RUNCMD_ARGS_MAX + 1];
		int r;
			
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
		
		/* build command arguments vector */
		argc = 0;
		r = split_quoted(cmd, &argc, argv, RUNCMD_ARGS_MAX);
		if ( r < 0 ) {
			fd_printf(STDERR_FILENO, "Cannot parse command\n");
			exit(RUNCMD_EXEC_FAIL);
		}
		r = split_quoted(args_extra, &argc, argv, RUNCMD_ARGS_MAX);
		if ( r < 0 ) {
			fd_printf(STDERR_FILENO, "Cannot parse extra args\n");
			exit(RUNCMD_EXEC_FAIL);
 		}
		if ( argc < 1 ) {
			fd_printf(STDERR_FILENO, "No command given\n");
			exit(RUNCMD_EXEC_FAIL);
		}	
		argv[argc] = NULL;
			
 		/* run extenral command */
		fd_printf(STDERR_FILENO, "$ %s %s\n", cmd, args_extra);
 		establish_child_signal_handlers();
 		sigprocmask(SIG_SETMASK, &sigm_old, NULL);
		execvp(argv[0], argv);

		fd_printf(STDERR_FILENO, "exec: %s\n", strerror(errno));
		exit(RUNCMD_EXEC_FAIL);
 	}
 }
