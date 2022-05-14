int lxc_attach(const char* name, const char* lxcpath, lxc_attach_exec_t exec_function, void* exec_payload, lxc_attach_options_t* options, pid_t* attached_process)
{
	int ret, status;
	pid_t init_pid, pid, attached_pid, expected;
	struct lxc_proc_context_info *init_ctx;
 	char* cwd;
 	char* new_cwd;
 	int ipc_sockets[2];
 	signed long personality;
 
 	if (!options)
		options = &attach_static_default_options;

	init_pid = lxc_cmd_get_init_pid(name, lxcpath);
	if (init_pid < 0) {
		ERROR("failed to get the init pid");
		return -1;
	}

	init_ctx = lxc_proc_get_context_info(init_pid);
	if (!init_ctx) {
		ERROR("failed to get context of the init process, pid = %ld", (long)init_pid);
		return -1;
	}

	personality = get_personality(name, lxcpath);
	if (init_ctx->personality < 0) {
		ERROR("Failed to get personality of the container");
		lxc_proc_put_context_info(init_ctx);
		return -1;
	}
	init_ctx->personality = personality;

	init_ctx->container = lxc_container_new(name, lxcpath);
	if (!init_ctx->container)
		return -1;

	if (!fetch_seccomp(init_ctx->container, options))
		WARN("Failed to get seccomp policy");

	if (!no_new_privs(init_ctx->container, options))
		WARN("Could not determine whether PR_SET_NO_NEW_PRIVS is set.");

	cwd = getcwd(NULL, 0);

	/* determine which namespaces the container was created with
	 * by asking lxc-start, if necessary
	 */
	if (options->namespaces == -1) {
		options->namespaces = lxc_cmd_get_clone_flags(name, lxcpath);
		/* call failed */
		if (options->namespaces == -1) {
			ERROR("failed to automatically determine the "
			      "namespaces which the container unshared");
			free(cwd);
			lxc_proc_put_context_info(init_ctx);
			return -1;
		}
	}

	/* create a socket pair for IPC communication; set SOCK_CLOEXEC in order
	 * to make sure we don't irritate other threads that want to fork+exec away
	 *
	 * IMPORTANT: if the initial process is multithreaded and another call
	 * just fork()s away without exec'ing directly after, the socket fd will
	 * exist in the forked process from the other thread and any close() in
	 * our own child process will not really cause the socket to close properly,
	 * potentiall causing the parent to hang.
	 *
	 * For this reason, while IPC is still active, we have to use shutdown()
	 * if the child exits prematurely in order to signal that the socket
	 * is closed and cannot assume that the child exiting will automatically
	 * do that.
	 *
	 * IPC mechanism: (X is receiver)
	 *   initial process        intermediate          attached
	 *        X           <---  send pid of
	 *                          attached proc,
	 *                          then exit
	 *    send 0 ------------------------------------>    X
	 *                                              [do initialization]
 	 *        X  <------------------------------------  send 1
 	 *   [add to cgroup, ...]
 	 *    send 2 ------------------------------------>    X
	 *						[set LXC_ATTACH_NO_NEW_PRIVS]
	 *        X  <------------------------------------  send 3
	 *   [open LSM label fd]
	 *    send 4 ------------------------------------>    X
	 *   						[set LSM label]
 	 *   close socket                                 close socket
 	 *                                                run program
 	 */
	ret = socketpair(PF_LOCAL, SOCK_STREAM | SOCK_CLOEXEC, 0, ipc_sockets);
	if (ret < 0) {
		SYSERROR("could not set up required IPC mechanism for attaching");
		free(cwd);
		lxc_proc_put_context_info(init_ctx);
		return -1;
	}

	/* create intermediate subprocess, three reasons:
	 *       1. runs all pthread_atfork handlers and the
	 *          child will no longer be threaded
	 *          (we can't properly setns() in a threaded process)
	 *       2. we can't setns() in the child itself, since
	 *          we want to make sure we are properly attached to
	 *          the pidns
	 *       3. also, the initial thread has to put the attached
	 *          process into the cgroup, which we can only do if
	 *          we didn't already setns() (otherwise, user
	 *          namespaces will hate us)
	 */
	pid = fork();

	if (pid < 0) {
		SYSERROR("failed to create first subprocess");
		free(cwd);
		lxc_proc_put_context_info(init_ctx);
		return -1;
 	}
 
 	if (pid) {
		int procfd = -1;
 		pid_t to_cleanup_pid = pid;
 
 		/* initial thread, we close the socket that is for the
		 * subprocesses
		 */
		close(ipc_sockets[1]);
		free(cwd);

		/* attach to cgroup, if requested */
		if (options->attach_flags & LXC_ATTACH_MOVE_TO_CGROUP) {
			if (!cgroup_attach(name, lxcpath, pid))
 				goto cleanup_error;
 		}
 
		/* Open /proc before setns() to the containers namespace so we
		 * don't rely on any information from inside the container.
		 */
		procfd = open("/proc", O_DIRECTORY | O_RDONLY | O_CLOEXEC);
		if (procfd < 0) {
			SYSERROR("Unable to open /proc.");
			goto cleanup_error;
		}

 		/* Let the child process know to go ahead */
 		status = 0;
 		ret = lxc_write_nointr(ipc_sockets[0], &status, sizeof(status));
		if (ret <= 0) {
			ERROR("error using IPC to notify attached process for initialization (0)");
			goto cleanup_error;
		}

		/* get pid from intermediate process */
		ret = lxc_read_nointr_expect(ipc_sockets[0], &attached_pid, sizeof(attached_pid), NULL);
		if (ret <= 0) {
			if (ret != 0)
				ERROR("error using IPC to receive pid of attached process");
			goto cleanup_error;
		}

		/* ignore SIGKILL (CTRL-C) and SIGQUIT (CTRL-\) - issue #313 */
		if (options->stdin_fd == 0) {
			signal(SIGINT, SIG_IGN);
			signal(SIGQUIT, SIG_IGN);
		}

		/* reap intermediate process */
		ret = wait_for_pid(pid);
		if (ret < 0)
			goto cleanup_error;

		/* we will always have to reap the grandchild now */
		to_cleanup_pid = attached_pid;

		/* tell attached process it may start initializing */
		status = 0;
		ret = lxc_write_nointr(ipc_sockets[0], &status, sizeof(status));
		if (ret <= 0) {
			ERROR("error using IPC to notify attached process for initialization (0)");
			goto cleanup_error;
		}

		/* wait for the attached process to finish initializing */
		expected = 1;
 		ret = lxc_read_nointr_expect(ipc_sockets[0], &status, sizeof(status), &expected);
 		if (ret <= 0) {
 			if (ret != 0)
				ERROR("error using IPC to receive notification "
				      "from attached process (1)");
 			goto cleanup_error;
 		}
 
 		/* tell attached process we're done */
 		status = 2;
 		ret = lxc_write_nointr(ipc_sockets[0], &status, sizeof(status));
 		if (ret <= 0) {
			ERROR("Error using IPC to notify attached process for "
			      "initialization (2): %s.", strerror(errno));
 			goto cleanup_error;
 		}
 
		/* Wait for the (grand)child to tell us that it's ready to set
		 * up its LSM labels.
		 */
		expected = 3;
		ret = lxc_read_nointr_expect(ipc_sockets[0], &status, sizeof(status), &expected);
		if (ret <= 0) {
			ERROR("Error using IPC for the child to tell us to open LSM fd (3): %s.",
			      strerror(errno));
			goto cleanup_error;
		}

		/* Open LSM fd and send it to child. */
		if ((options->namespaces & CLONE_NEWNS) && (options->attach_flags & LXC_ATTACH_LSM) && init_ctx->lsm_label) {
			int on_exec, labelfd;
			on_exec = options->attach_flags & LXC_ATTACH_LSM_EXEC ? 1 : 0;
			/* Open fd for the LSM security module. */
			labelfd = lsm_openat(procfd, attached_pid, on_exec);
			if (labelfd < 0)
				goto cleanup_error;

			/* Send child fd of the LSM security module to write to. */
			ret = lxc_abstract_unix_send_fd(ipc_sockets[0], labelfd, NULL, 0);
			if (ret <= 0) {
				ERROR("Error using IPC to send child LSM fd (4): %s.",
						strerror(errno));
				goto cleanup_error;
			}
		}

 		/* now shut down communication with child, we're done */
 		shutdown(ipc_sockets[0], SHUT_RDWR);
 		close(ipc_sockets[0]);
		lxc_proc_put_context_info(init_ctx);

		/* we're done, the child process should now execute whatever
		 * it is that the user requested. The parent can now track it
		 * with waitpid() or similar.
		 */

		*attached_process = attached_pid;
		return 0;

	cleanup_error:
 		/* first shut down the socket, then wait for the pid,
 		 * otherwise the pid we're waiting for may never exit
 		 */
		if (procfd >= 0)
			close(procfd);
 		shutdown(ipc_sockets[0], SHUT_RDWR);
 		close(ipc_sockets[0]);
 		if (to_cleanup_pid)
			(void) wait_for_pid(to_cleanup_pid);
		lxc_proc_put_context_info(init_ctx);
		return -1;
	}

	/* first subprocess begins here, we close the socket that is for the
	 * initial thread
	 */
	close(ipc_sockets[0]);

	/* Wait for the parent to have setup cgroups */
	expected = 0;
	status = -1;
	ret = lxc_read_nointr_expect(ipc_sockets[1], &status, sizeof(status), &expected);
	if (ret <= 0) {
		ERROR("error communicating with child process");
		shutdown(ipc_sockets[1], SHUT_RDWR);
		rexit(-1);
	}

 	if ((options->attach_flags & LXC_ATTACH_MOVE_TO_CGROUP) && cgns_supported())
 		options->namespaces |= CLONE_NEWCGROUP;
 
 	/* attach now, create another subprocess later, since pid namespaces
 	 * only really affect the children of the current process
 	 */
	ret = lxc_attach_to_ns(init_pid, options->namespaces);
	if (ret < 0) {
		ERROR("failed to enter the namespace");
		shutdown(ipc_sockets[1], SHUT_RDWR);
		rexit(-1);
	}

	/* attach succeeded, try to cwd */
	if (options->initial_cwd)
		new_cwd = options->initial_cwd;
	else
		new_cwd = cwd;
	ret = chdir(new_cwd);
	if (ret < 0)
		WARN("could not change directory to '%s'", new_cwd);
	free(cwd);

	/* now create the real child process */
	{
		struct attach_clone_payload payload = {
			.ipc_socket = ipc_sockets[1],
			.options = options,
 			.init_ctx = init_ctx,
 			.exec_function = exec_function,
 			.exec_payload = exec_payload,
 		};
 		/* We use clone_parent here to make this subprocess a direct child of
 		 * the initial process. Then this intermediate process can exit and
		 * the parent can directly track the attached process.
		 */
		pid = lxc_clone(attach_child_main, &payload, CLONE_PARENT);
	}

	/* shouldn't happen, clone() should always return positive pid */
	if (pid <= 0) {
		SYSERROR("failed to create subprocess");
		shutdown(ipc_sockets[1], SHUT_RDWR);
		rexit(-1);
	}

	/* tell grandparent the pid of the pid of the newly created child */
	ret = lxc_write_nointr(ipc_sockets[1], &pid, sizeof(pid));
	if (ret != sizeof(pid)) {
		/* if this really happens here, this is very unfortunate, since the
		 * parent will not know the pid of the attached process and will
		 * not be able to wait for it (and we won't either due to CLONE_PARENT)
		 * so the parent won't be able to reap it and the attached process
		 * will remain a zombie
		 */
		ERROR("error using IPC to notify main process of pid of the attached process");
		shutdown(ipc_sockets[1], SHUT_RDWR);
		rexit(-1);
	}

	/* the rest is in the hands of the initial and the attached process */
	rexit(0);
}
