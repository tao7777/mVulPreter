struct key *request_key_and_link(struct key_type *type,
				 const char *description,
				 const void *callout_info,
				 size_t callout_len,
				 void *aux,
				 struct key *dest_keyring,
				 unsigned long flags)
{
	struct keyring_search_context ctx = {
 		.index_key.type		= type,
 		.index_key.description	= description,
 		.cred			= current_cred(),
		.match_data.cmp		= key_default_cmp,
 		.match_data.raw_data	= description,
 		.match_data.lookup_type	= KEYRING_SEARCH_LOOKUP_DIRECT,
 	};
	struct key *key;
	key_ref_t key_ref;
	int ret;

	kenter("%s,%s,%p,%zu,%p,%p,%lx",
	       ctx.index_key.type->name, ctx.index_key.description,
	       callout_info, callout_len, aux, dest_keyring, flags);

	if (type->match_preparse) {
		ret = type->match_preparse(&ctx.match_data);
		if (ret < 0) {
			key = ERR_PTR(ret);
			goto error;
		}
	}

	/* search all the process keyrings for a key */
	key_ref = search_process_keyrings(&ctx);

	if (!IS_ERR(key_ref)) {
		key = key_ref_to_ptr(key_ref);
		if (dest_keyring) {
			construct_get_dest_keyring(&dest_keyring);
			ret = key_link(dest_keyring, key);
			key_put(dest_keyring);
			if (ret < 0) {
				key_put(key);
				key = ERR_PTR(ret);
				goto error_free;
			}
		}
	} else if (PTR_ERR(key_ref) != -EAGAIN) {
		key = ERR_CAST(key_ref);
	} else  {
		/* the search failed, but the keyrings were searchable, so we
		 * should consult userspace if we can */
		key = ERR_PTR(-ENOKEY);
		if (!callout_info)
			goto error_free;

		key = construct_key_and_link(&ctx, callout_info, callout_len,
					     aux, dest_keyring, flags);
	}

error_free:
	if (type->match_free)
		type->match_free(&ctx.match_data);
error:
	kleave(" = %p", key);
	return key;
}
