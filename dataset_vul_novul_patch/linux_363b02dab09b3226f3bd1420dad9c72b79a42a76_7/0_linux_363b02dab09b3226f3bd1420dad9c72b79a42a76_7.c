int key_update(key_ref_t key_ref, const void *payload, size_t plen)
{
	struct key_preparsed_payload prep;
	struct key *key = key_ref_to_ptr(key_ref);
	int ret;

	key_check(key);

	/* the key must be writable */
	ret = key_permission(key_ref, KEY_NEED_WRITE);
	if (ret < 0)
		return ret;

	/* attempt to update it if supported */
	if (!key->type->update)
		return -EOPNOTSUPP;

	memset(&prep, 0, sizeof(prep));
	prep.data = payload;
	prep.datalen = plen;
	prep.quotalen = key->type->def_datalen;
	prep.expiry = TIME_T_MAX;
	if (key->type->preparse) {
		ret = key->type->preparse(&prep);
		if (ret < 0)
			goto error;
	}

	down_write(&key->sem);
 
 	ret = key->type->update(key, &prep);
 	if (ret == 0)
		/* Updating a negative key positively instantiates it */
		mark_key_instantiated(key, 0);
 
 	up_write(&key->sem);
 
error:
	if (key->type->preparse)
		key->type->free_preparse(&prep);
	return ret;
}
