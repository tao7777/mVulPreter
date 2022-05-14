char *auth_server(int f_in, int f_out, int module, const char *host,
		  const char *addr, const char *leader)
{
	char *users = lp_auth_users(module);
	char challenge[MAX_DIGEST_LEN*2];
	char line[BIGPATHBUFLEN];
	char **auth_uid_groups = NULL;
	int auth_uid_groups_cnt = -1;
	const char *err = NULL;
	int group_match = -1;
	char *tok, *pass;
	char opt_ch = '\0';

	/* if no auth list then allow anyone in! */
	if (!users || !*users)
        if (!users || !*users)
                return "";
 
        gen_challenge(addr, challenge);
 
        io_printf(f_out, "%s%s\n", leader, challenge);
		return NULL;
	}
