int cap_bprm_set_creds(struct linux_binprm *bprm)
{
	const struct cred *old = current_cred();
	struct cred *new = bprm->cred;
	bool effective, has_cap = false;
	int ret;

	effective = false;
	ret = get_file_caps(bprm, &effective, &has_cap);
	if (ret < 0)
		return ret;

	if (!issecure(SECURE_NOROOT)) {
		/*
		 * If the legacy file capability is set, then don't set privs
		 * for a setuid root binary run by a non-root user.  Do set it
		 * for a root user just to cause least surprise to an admin.
		 */
		if (has_cap && new->uid != 0 && new->euid == 0) {
			warn_setuid_and_fcaps_mixed(bprm->filename);
			goto skip;
		}
		/*
		 * To support inheritance of root-permissions and suid-root
		 * executables under compatibility mode, we override the
		 * capability sets for the file.
		 *
		 * If only the real uid is 0, we do not set the effective bit.
		 */
		if (new->euid == 0 || new->uid == 0) {
			/* pP' = (cap_bset & ~0) | (pI & ~0) */
			new->cap_permitted = cap_combine(old->cap_bset,
							 old->cap_inheritable);
		}
		if (new->euid == 0)
			effective = true;
 	}
 skip:
 
 	/* Don't let someone trace a set[ug]id/setpcap binary with the revised
 	 * credentials unless they have the appropriate permit
 	 */
	if ((new->euid != old->uid ||
	     new->egid != old->gid ||
	     !cap_issubset(new->cap_permitted, old->cap_permitted)) &&
	    bprm->unsafe & ~LSM_UNSAFE_PTRACE_CAP) {
		/* downgrade; they get no more than they had, and maybe less */
		if (!capable(CAP_SETUID)) {
			new->euid = new->uid;
			new->egid = new->gid;
		}
		new->cap_permitted = cap_intersect(new->cap_permitted,
						   old->cap_permitted);
	}

	new->suid = new->fsuid = new->euid;
	new->sgid = new->fsgid = new->egid;

	if (effective)
		new->cap_effective = new->cap_permitted;
	else
		cap_clear(new->cap_effective);
	bprm->cap_effective = effective;

	/*
	 * Audit candidate if current->cap_effective is set
	 *
	 * We do not bother to audit if 3 things are true:
	 *   1) cap_effective has all caps
	 *   2) we are root
	 *   3) root is supposed to have all caps (SECURE_NOROOT)
	 * Since this is just a normal root execing a process.
	 *
	 * Number 1 above might fail if you don't have a full bset, but I think
	 * that is interesting information to audit.
	 */
	if (!cap_isclear(new->cap_effective)) {
		if (!cap_issubset(CAP_FULL_SET, new->cap_effective) ||
		    new->euid != 0 || new->uid != 0 ||
		    issecure(SECURE_NOROOT)) {
			ret = audit_log_bprm_fcaps(bprm, new, old);
			if (ret < 0)
				return ret;
		}
	}

	new->securebits &= ~issecure_mask(SECURE_KEEP_CAPS);
	return 0;
}
