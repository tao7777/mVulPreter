 static bool caller_is_in_ancestor(pid_t pid, const char *contrl, const char *cg, char **nextcg)
 {
 	bool answer = false;
	char *c2 = get_pid_cgroup(pid, contrl);
	char *linecmp;
 
	if (!c2)
 		return false;
	prune_init_slice(c2);
 
	/*
	 * callers pass in '/' for root cgroup, otherwise they pass
	 * in a cgroup without leading '/'
	 */
	linecmp = *cg == '/' ? c2 : c2+1;
	if (strncmp(linecmp, cg, strlen(linecmp)) != 0) {
		if (nextcg) {
			*nextcg = get_next_cgroup_dir(linecmp, cg);
 		}
		goto out;
	}
	answer = true;

out:
	free(c2);
	return answer;
}

/*
 * If caller is in /a/b/c, he may see that /a exists, but not /b or /a/c.
 */
static bool caller_may_see_dir(pid_t pid, const char *contrl, const char *cg)
{
	bool answer = false;
	char *c2, *task_cg;
	size_t target_len, task_len;

	if (strcmp(cg, "/") == 0)
		return true;

	c2 = get_pid_cgroup(pid, contrl);

	if (!c2)
		return false;

	task_cg = c2 + 1;
	target_len = strlen(cg);
	task_len = strlen(task_cg);
	if (strcmp(cg, task_cg) == 0) {
 		answer = true;
 		goto out;
 	}
	if (target_len < task_len) {
		/* looking up a parent dir */
		if (strncmp(task_cg, cg, target_len) == 0 && task_cg[target_len] == '/')
			answer = true;
		goto out;
	}
	if (target_len > task_len) {
		/* looking up a child dir */
		if (strncmp(task_cg, cg, task_len) == 0 && cg[task_len] == '/')
			answer = true;
		goto out;
	}
 
 out:
	free(c2);
 	return answer;
 }
