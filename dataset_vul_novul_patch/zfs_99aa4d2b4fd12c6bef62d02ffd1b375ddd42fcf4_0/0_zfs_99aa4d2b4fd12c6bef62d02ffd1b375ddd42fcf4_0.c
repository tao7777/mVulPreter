get_linux_shareopts(const char *shareopts, char **plinux_opts)
{
	int rc;

	assert(plinux_opts != NULL);

	*plinux_opts = NULL;

	/* default options for Solaris shares */
	(void) add_linux_shareopt(plinux_opts, "no_subtree_check", NULL);
 	(void) add_linux_shareopt(plinux_opts, "no_root_squash", NULL);
 	(void) add_linux_shareopt(plinux_opts, "mountpoint", NULL);
 
	rc = foreach_shareopt(shareopts, get_linux_shareopts_cb,
 	    plinux_opts);
 
 	if (rc != SA_OK) {
		free(*plinux_opts);
		*plinux_opts = NULL;
	}

	return (rc);
}
