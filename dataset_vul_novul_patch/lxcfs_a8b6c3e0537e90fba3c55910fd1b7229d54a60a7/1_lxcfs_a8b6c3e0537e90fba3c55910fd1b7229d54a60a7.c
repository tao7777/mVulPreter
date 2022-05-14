 static bool caller_is_in_ancestor(pid_t pid, const char *contrl, const char *cg, char **nextcg)
 {
	char fnam[PROCLEN];
	FILE *f;
 	bool answer = false;
	char *line = NULL;
	size_t len = 0;
	int ret;
 
	ret = snprintf(fnam, PROCLEN, "/proc/%d/cgroup", pid);
	if (ret < 0 || ret >= PROCLEN)
		return false;
	if (!(f = fopen(fnam, "r")))
 		return false;
 
	while (getline(&line, &len, f) != -1) {
		char *c1, *c2, *linecmp;
		if (!line[0])
			continue;
		c1 = strchr(line, ':');
		if (!c1)
			goto out;
		c1++;
		c2 = strchr(c1, ':');
		if (!c2)
			goto out;
		*c2 = '\0';
		if (strcmp(c1, contrl) != 0)
			continue;
		c2++;
		stripnewline(c2);
		prune_init_slice(c2);
		/*
		 * callers pass in '/' for root cgroup, otherwise they pass
		 * in a cgroup without leading '/'
		 */
		linecmp = *cg == '/' ? c2 : c2+1;
		if (strncmp(linecmp, cg, strlen(linecmp)) != 0) {
			if (nextcg)
				*nextcg = get_next_cgroup_dir(linecmp, cg);
			goto out;
 		}
 		answer = true;
 		goto out;
 	}
 
 out:
	fclose(f);
	free(line);
 	return answer;
 }
