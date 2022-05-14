void free_bprm(struct linux_binprm *bprm)
{
	free_arg_pages(bprm);
	if (bprm->cred) {
 		mutex_unlock(&current->signal->cred_guard_mutex);
 		abort_creds(bprm->cred);
 	}
 	kfree(bprm);
 }
