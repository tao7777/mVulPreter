 process_add_smartcard_key(SocketEntry *e)
 {
	char *provider = NULL, *pin, canonical_provider[PATH_MAX];
 	int r, i, version, count = 0, success = 0, confirm = 0;
 	u_int seconds;
 	time_t death = 0;
	u_char type;
	struct sshkey **keys = NULL, *k;
	Identity *id;
	Idtab *tab;

	if ((r = sshbuf_get_cstring(e->request, &provider, NULL)) != 0 ||
	    (r = sshbuf_get_cstring(e->request, &pin, NULL)) != 0)
		fatal("%s: buffer error: %s", __func__, ssh_err(r));

	while (sshbuf_len(e->request)) {
		if ((r = sshbuf_get_u8(e->request, &type)) != 0)
			fatal("%s: buffer error: %s", __func__, ssh_err(r));
		switch (type) {
		case SSH_AGENT_CONSTRAIN_LIFETIME:
			if ((r = sshbuf_get_u32(e->request, &seconds)) != 0)
				fatal("%s: buffer error: %s",
				    __func__, ssh_err(r));
			death = monotime() + seconds;
			break;
		case SSH_AGENT_CONSTRAIN_CONFIRM:
			confirm = 1;
			break;
		default:
			error("process_add_smartcard_key: "
			    "Unknown constraint type %d", type);
 			goto send;
 		}
 	}
	if (realpath(provider, canonical_provider) == NULL) {
		verbose("failed PKCS#11 add of \"%.100s\": realpath: %s",
		    provider, strerror(errno));
		goto send;
	}
	if (match_pattern_list(canonical_provider, pkcs11_whitelist, 0) != 1) {
		verbose("refusing PKCS#11 add of \"%.100s\": "
		    "provider not whitelisted", canonical_provider);
		goto send;
	}
	debug("%s: add %.100s", __func__, canonical_provider);
 	if (lifetime && !death)
 		death = monotime() + lifetime;
 
	count = pkcs11_add_provider(canonical_provider, pin, &keys);
 	for (i = 0; i < count; i++) {
 		k = keys[i];
 		version = k->type == KEY_RSA1 ? 1 : 2;
 		tab = idtab_lookup(version);
 		if (lookup_identity(k, version) == NULL) {
 			id = xcalloc(1, sizeof(Identity));
 			id->key = k;
			id->provider = xstrdup(canonical_provider);
			id->comment = xstrdup(canonical_provider); /* XXX */
 			id->death = death;
 			id->confirm = confirm;
 			TAILQ_INSERT_TAIL(&tab->idlist, id, next);
			tab->nentries++;
			success = 1;
		} else {
			sshkey_free(k);
		}
		keys[i] = NULL;
	}
send:
	free(pin);
	free(provider);
	free(keys);
	send_status(e, success);
}
