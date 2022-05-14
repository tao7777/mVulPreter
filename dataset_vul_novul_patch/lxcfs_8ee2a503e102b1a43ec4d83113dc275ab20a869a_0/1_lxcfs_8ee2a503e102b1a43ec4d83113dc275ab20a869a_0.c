static bool do_write_pids(pid_t tpid, const char *contrl, const char *cg, const char *file, const char *buf)
 {
 	int sock[2] = {-1, -1};
 	pid_t qpid, cpid = -1;
	FILE *pids_file = NULL;
	bool answer = false, fail = false;

	pids_file = open_pids_file(contrl, cg);
	if (!pids_file)
		return false;

	/*
	 * write the pids to a socket, have helper in writer's pidns
	 * call movepid for us
	 */
	if (socketpair(AF_UNIX, SOCK_DGRAM, 0, sock) < 0) {
		perror("socketpair");
		goto out;
	}

	cpid = fork();
	if (cpid == -1)
		goto out;

	if (!cpid) { // child
		fclose(pids_file);
		pid_from_ns_wrapper(sock[1], tpid);
	}

	const char *ptr = buf;
	while (sscanf(ptr, "%d", &qpid) == 1) {
		struct ucred cred;
		char v;

		if (write(sock[0], &qpid, sizeof(qpid)) != sizeof(qpid)) {
			fprintf(stderr, "%s: error writing pid to child: %s\n",
				__func__, strerror(errno));
			goto out;
		}
 
 		if (recv_creds(sock[0], &cred, &v)) {
 			if (v == '0') {
 				if (fprintf(pids_file, "%d", (int) cred.pid) < 0)
 					fail = true;
 			}
		}

		ptr = strchr(ptr, '\n');
		if (!ptr)
			break;
		ptr++;
	}

	/* All good, write the value */
	qpid = -1;
	if (write(sock[0], &qpid ,sizeof(qpid)) != sizeof(qpid))
		fprintf(stderr, "Warning: failed to ask child to exit\n");

	if (!fail)
		answer = true;

out:
	if (cpid != -1)
		wait_for_pid(cpid);
	if (sock[0] != -1) {
		close(sock[0]);
		close(sock[1]);
	}
	if (pids_file) {
		if (fclose(pids_file) != 0)
			answer = false;
	}
	return answer;
}
