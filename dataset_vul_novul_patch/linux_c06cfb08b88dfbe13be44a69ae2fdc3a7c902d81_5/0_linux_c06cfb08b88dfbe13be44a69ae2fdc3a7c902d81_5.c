struct key *key_get_instantiation_authkey(key_serial_t target_id)
{
	char description[16];
	struct keyring_search_context ctx = {
 		.index_key.type		= &key_type_request_key_auth,
 		.index_key.description	= description,
 		.cred			= current_cred(),
		.match_data.cmp		= key_default_cmp,
 		.match_data.raw_data	= description,
 		.match_data.lookup_type	= KEYRING_SEARCH_LOOKUP_DIRECT,
 	};
	struct key *authkey;
	key_ref_t authkey_ref;

	sprintf(description, "%x", target_id);

	authkey_ref = search_process_keyrings(&ctx);

	if (IS_ERR(authkey_ref)) {
		authkey = ERR_CAST(authkey_ref);
		if (authkey == ERR_PTR(-EAGAIN))
			authkey = ERR_PTR(-ENOKEY);
		goto error;
	}

	authkey = key_ref_to_ptr(authkey_ref);
	if (test_bit(KEY_FLAG_REVOKED, &authkey->flags)) {
		key_put(authkey);
		authkey = ERR_PTR(-EKEYREVOKED);
	}

error:
	return authkey;
}
