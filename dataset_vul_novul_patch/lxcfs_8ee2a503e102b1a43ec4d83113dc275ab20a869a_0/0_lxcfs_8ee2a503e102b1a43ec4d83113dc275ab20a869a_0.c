static bool do_write_pids(pid_t tpid, const char *contrl, const char *cg, const char *file, const char *buf)
/*
 * Given host @uid, return the uid to which it maps in
 * @pid's user namespace, or -1 if none.
 */
bool hostuid_to_ns(uid_t uid, pid_t pid, uid_t *answer)
{
	FILE *f;
	char line[400];

	sprintf(line, "/proc/%d/uid_map", pid);
	if ((f = fopen(line, "r")) == NULL) {
		return false;
	}

	*answer = convert_id_to_ns(f, uid);
	fclose(f);

	if (*answer == -1)
		return false;
	return true;
}

/*
 * get_pid_creds: get the real uid and gid of @pid from
 * /proc/$$/status
 * (XXX should we use euid here?)
 */
void get_pid_creds(pid_t pid, uid_t *uid, gid_t *gid)
{
	char line[400];
	uid_t u;
	gid_t g;
	FILE *f;

	*uid = -1;
	*gid = -1;
	sprintf(line, "/proc/%d/status", pid);
	if ((f = fopen(line, "r")) == NULL) {
		fprintf(stderr, "Error opening %s: %s\n", line, strerror(errno));
		return;
	}
	while (fgets(line, 400, f)) {
		if (strncmp(line, "Uid:", 4) == 0) {
			if (sscanf(line+4, "%u", &u) != 1) {
				fprintf(stderr, "bad uid line for pid %u\n", pid);
				fclose(f);
				return;
			}
			*uid = u;
		} else if (strncmp(line, "Gid:", 4) == 0) {
			if (sscanf(line+4, "%u", &g) != 1) {
				fprintf(stderr, "bad gid line for pid %u\n", pid);
				fclose(f);
				return;
			}
			*gid = g;
		}
	}
	fclose(f);
}

/*
 * May the requestor @r move victim @v to a new cgroup?
 * This is allowed if
 *   . they are the same task
 *   . they are ownedy by the same uid
 *   . @r is root on the host, or
 *   . @v's uid is mapped into @r's where @r is root.
 */
bool may_move_pid(pid_t r, uid_t r_uid, pid_t v)
{
	uid_t v_uid, tmpuid;
	gid_t v_gid;

	if (r == v)
		return true;
	if (r_uid == 0)
		return true;
	get_pid_creds(v, &v_uid, &v_gid);
	if (r_uid == v_uid)
		return true;
	if (hostuid_to_ns(r_uid, r, &tmpuid) && tmpuid == 0
			&& hostuid_to_ns(v_uid, r, &tmpuid))
		return true;
	return false;
}

static bool do_write_pids(pid_t tpid, uid_t tuid, const char *contrl, const char *cg,
		const char *file, const char *buf)
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
				if (!may_move_pid(tpid, tuid, cred.pid)) {
					fail = true;
					break;
				}
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
