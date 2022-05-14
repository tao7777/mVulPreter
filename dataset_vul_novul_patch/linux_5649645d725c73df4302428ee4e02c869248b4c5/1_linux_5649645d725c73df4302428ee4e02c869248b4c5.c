SYSCALL_DEFINE5(add_key, const char __user *, _type,
		const char __user *, _description,
		const void __user *, _payload,
		size_t, plen,
		key_serial_t, ringid)
{
	key_ref_t keyring_ref, key_ref;
	char type[32], *description;
	void *payload;
	long ret;

	ret = -EINVAL;
	if (plen > 1024 * 1024 - 1)
		goto error;

	/* draw all the data into kernel space */
	ret = key_get_type_from_user(type, _type, sizeof(type));
	if (ret < 0)
		goto error;

	description = NULL;
	if (_description) {
		description = strndup_user(_description, KEY_MAX_DESC_SIZE);
		if (IS_ERR(description)) {
			ret = PTR_ERR(description);
			goto error;
		}
		if (!*description) {
			kfree(description);
			description = NULL;
		} else if ((description[0] == '.') &&
			   (strncmp(type, "keyring", 7) == 0)) {
			ret = -EPERM;
			goto error2;
		}
	}

 	/* pull the payload in if one was supplied */
 	payload = NULL;
 
	if (_payload) {
 		ret = -ENOMEM;
 		payload = kvmalloc(plen, GFP_KERNEL);
 		if (!payload)
			goto error2;

		ret = -EFAULT;
		if (copy_from_user(payload, _payload, plen) != 0)
			goto error3;
	}

	/* find the target keyring (which must be writable) */
	keyring_ref = lookup_user_key(ringid, KEY_LOOKUP_CREATE, KEY_NEED_WRITE);
	if (IS_ERR(keyring_ref)) {
		ret = PTR_ERR(keyring_ref);
		goto error3;
	}

	/* create or update the requested key and add it to the target
	 * keyring */
	key_ref = key_create_or_update(keyring_ref, type, description,
				       payload, plen, KEY_PERM_UNDEF,
				       KEY_ALLOC_IN_QUOTA);
	if (!IS_ERR(key_ref)) {
		ret = key_ref_to_ptr(key_ref)->serial;
		key_ref_put(key_ref);
	}
	else {
		ret = PTR_ERR(key_ref);
	}

	key_ref_put(keyring_ref);
 error3:
	kvfree(payload);
 error2:
	kfree(description);
 error:
	return ret;
}
