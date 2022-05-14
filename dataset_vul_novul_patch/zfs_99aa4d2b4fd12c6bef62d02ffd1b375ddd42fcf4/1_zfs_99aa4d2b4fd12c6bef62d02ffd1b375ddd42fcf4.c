foreach_nfs_shareopt(const char *shareopts,
    nfs_shareopt_callback_t callback, void *cookie)
{
	char *shareopts_dup, *opt, *cur, *value;
	int was_nul, rc;
	if (shareopts == NULL)
		return (SA_OK);
	shareopts_dup = strdup(shareopts);
	if (shareopts_dup == NULL)
		return (SA_NO_MEMORY);
	opt = shareopts_dup;
	was_nul = 0;
	while (1) {
		cur = opt;
		while (*cur != ',' && *cur != '\0')
			cur++;
		if (*cur == '\0')
			was_nul = 1;
		*cur = '\0';
		if (cur > opt) {
			value = strchr(opt, '=');
			if (value != NULL) {
				*value = '\0';
				value++;
			}
			rc = callback(opt, value, cookie);
			if (rc != SA_OK) {
				free(shareopts_dup);
				return (rc);
			}
		}
		opt = cur + 1;
		if (was_nul)
			break;
	}
	free(shareopts_dup);
	return (0);
}
