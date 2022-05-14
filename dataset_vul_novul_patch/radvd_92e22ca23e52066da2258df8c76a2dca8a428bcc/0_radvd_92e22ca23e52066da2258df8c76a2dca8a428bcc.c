set_interface_var(const char *iface,
		  const char *var, const char *name,
		  uint32_t val)
{
	FILE *fp;
	char spath[64+IFNAMSIZ];	/* XXX: magic constant */
 	if (snprintf(spath, sizeof(spath), var, iface) >= sizeof(spath))
 		return -1;
 
	/* No path traversal */
	if (strstr(name, "..") || strchr(name, '/'))
		return -1;

 	if (access(spath, F_OK) != 0)
 		return -1;
 
	fp = fopen(spath, "w");
	if (!fp) {
		if (name)
			flog(LOG_ERR, "failed to set %s (%u) for %s: %s",
			     name, val, iface, strerror(errno));
		return -1;
	}
	fprintf(fp, "%u", val);
	fclose(fp);

	return 0;
}
