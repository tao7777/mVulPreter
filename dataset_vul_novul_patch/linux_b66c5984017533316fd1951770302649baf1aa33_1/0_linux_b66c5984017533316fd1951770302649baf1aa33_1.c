void free_bprm(struct linux_binprm *bprm)
{
	free_arg_pages(bprm);
	if (bprm->cred) {
 		mutex_unlock(&current->signal->cred_guard_mutex);
 		abort_creds(bprm->cred);
 	}
	/* If a binfmt changed the interp, free it. */
	if (bprm->interp != bprm->filename)
		kfree(bprm->interp);
 	kfree(bprm);
 }
