static const char *check_secret(int module, const char *user, const char *group,
				const char *challenge, const char *pass)
{
	char line[1024];
        char pass2[MAX_DIGEST_LEN*2];
        const char *fname = lp_secrets_file(module);
        STRUCT_STAT st;
       int ok = 1;
        int user_len = strlen(user);
        int group_len = group ? strlen(group) : 0;
        char *err;
       FILE *fh;
 
       if (!fname || !*fname || (fh = fopen(fname, "r")) == NULL)
                return "no secrets file";
 
       if (do_fstat(fileno(fh), &st) == -1) {
                rsyserr(FLOG, errno, "fstat(%s)", fname);
                ok = 0;
        } else if (lp_strict_modes(module)) {
			rprintf(FLOG, "secrets file must not be other-accessible (see strict modes option)\n");
			ok = 0;
		} else if (MY_UID() == 0 && st.st_uid != 0) {
			rprintf(FLOG, "secrets file must be owned by root when running as root (see strict modes)\n");
			ok = 0;
		}
                }
