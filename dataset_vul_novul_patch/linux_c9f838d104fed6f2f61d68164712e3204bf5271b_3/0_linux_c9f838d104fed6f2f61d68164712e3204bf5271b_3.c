 int install_thread_keyring_to_cred(struct cred *new)
 {
 	struct key *keyring;
 
	if (new->thread_keyring)
		return 0;

 	keyring = keyring_alloc("_tid", new->uid, new->gid, new,
 				KEY_POS_ALL | KEY_USR_VIEW,
 				KEY_ALLOC_QUOTA_OVERRUN,
				NULL, NULL);
	if (IS_ERR(keyring))
		return PTR_ERR(keyring);

	new->thread_keyring = keyring;
	return 0;
 }
