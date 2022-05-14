 static int install_thread_keyring(void)
 {
	struct cred *new;
	int ret;

	new = prepare_creds();
 	if (!new)
 		return -ENOMEM;
 
	BUG_ON(new->thread_keyring);
 	ret = install_thread_keyring_to_cred(new);
 	if (ret < 0) {
 		abort_creds(new);
		return ret;
	}

	return commit_creds(new);
 }
