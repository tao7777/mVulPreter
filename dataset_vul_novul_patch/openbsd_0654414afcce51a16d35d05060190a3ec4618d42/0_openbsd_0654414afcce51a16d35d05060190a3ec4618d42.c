int_x509_param_set_hosts(X509_VERIFY_PARAM_ID *id, int mode,
    const char *name, size_t namelen)
 {
 	char *copy;
 
	if (name != NULL && namelen == 0)
		namelen = strlen(name);
 	/*
 	 * Refuse names with embedded NUL bytes.
 	 * XXX: Do we need to push an error onto the error stack?
	 */
	if (name && memchr(name, '\0', namelen))
		return 0;

	if (mode == SET_HOST && id->hosts) {
		string_stack_free(id->hosts);
		id->hosts = NULL;
	}
	if (name == NULL || namelen == 0)
		return 1;
	copy = strndup(name, namelen);
	if (copy == NULL)
		return 0;

	if (id->hosts == NULL &&
	    (id->hosts = sk_OPENSSL_STRING_new_null()) == NULL) {
		free(copy);
		return 0;
	}

	if (!sk_OPENSSL_STRING_push(id->hosts, copy)) {
		free(copy);
		if (sk_OPENSSL_STRING_num(id->hosts) == 0) {
			sk_OPENSSL_STRING_free(id->hosts);
			id->hosts = NULL;
		}
		return 0;
	}

	return 1;
}
