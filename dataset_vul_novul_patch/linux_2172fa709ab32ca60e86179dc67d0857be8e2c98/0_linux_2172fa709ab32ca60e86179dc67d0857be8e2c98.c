static int security_context_to_sid_core(const char *scontext, u32 scontext_len,
					u32 *sid, u32 def_sid, gfp_t gfp_flags,
					int force)
{
	char *scontext2, *str = NULL;
 	struct context context;
 	int rc = 0;
 
	/* An empty security context is never valid. */
	if (!scontext_len)
		return -EINVAL;

 	if (!ss_initialized) {
 		int i;
 
		for (i = 1; i < SECINITSID_NUM; i++) {
			if (!strcmp(initial_sid_to_string[i], scontext)) {
				*sid = i;
				return 0;
			}
		}
		*sid = SECINITSID_KERNEL;
		return 0;
	}
	*sid = SECSID_NULL;

	/* Copy the string so that we can modify the copy as we parse it. */
	scontext2 = kmalloc(scontext_len + 1, gfp_flags);
	if (!scontext2)
		return -ENOMEM;
	memcpy(scontext2, scontext, scontext_len);
	scontext2[scontext_len] = 0;

	if (force) {
		/* Save another copy for storing in uninterpreted form */
		rc = -ENOMEM;
		str = kstrdup(scontext2, gfp_flags);
		if (!str)
			goto out;
	}

	read_lock(&policy_rwlock);
	rc = string_to_context_struct(&policydb, &sidtab, scontext2,
				      scontext_len, &context, def_sid);
	if (rc == -EINVAL && force) {
		context.str = str;
		context.len = scontext_len;
		str = NULL;
	} else if (rc)
		goto out_unlock;
	rc = sidtab_context_to_sid(&sidtab, &context, sid);
	context_destroy(&context);
out_unlock:
	read_unlock(&policy_rwlock);
out:
	kfree(scontext2);
	kfree(str);
	return rc;
}
