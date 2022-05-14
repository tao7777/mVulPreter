 static int install_process_keyring(void)
 {
	struct cred *new;
	int ret;

	new = prepare_creds();
	if (!new)
		return -ENOMEM;

 	ret = install_process_keyring_to_cred(new);
 	if (ret < 0) {
 		abort_creds(new);
		return ret;
 	}
 
 	return commit_creds(new);
 }
