userauth_hostbased(struct ssh *ssh)
{
	Authctxt *authctxt = ssh->authctxt;
	struct sshbuf *b;
	struct sshkey *key = NULL;
	char *pkalg, *cuser, *chost;
	u_char *pkblob, *sig;
 	size_t alen, blen, slen;
 	int r, pktype, authenticated = 0;
 
 	/* XXX use sshkey_froms() */
 	if ((r = sshpkt_get_cstring(ssh, &pkalg, &alen)) != 0 ||
 	    (r = sshpkt_get_string(ssh, &pkblob, &blen)) != 0 ||
	    (r = sshpkt_get_cstring(ssh, &chost, NULL)) != 0 ||
	    (r = sshpkt_get_cstring(ssh, &cuser, NULL)) != 0 ||
	    (r = sshpkt_get_string(ssh, &sig, &slen)) != 0)
		fatal("%s: packet parsing: %s", __func__, ssh_err(r));

	debug("%s: cuser %s chost %s pkalg %s slen %zu", __func__,
	    cuser, chost, pkalg, slen);
#ifdef DEBUG_PK
	debug("signature:");
	sshbuf_dump_data(sig, siglen, stderr);
#endif
	pktype = sshkey_type_from_name(pkalg);
	if (pktype == KEY_UNSPEC) {
		/* this is perfectly legal */
		logit("%s: unsupported public key algorithm: %s",
		    __func__, pkalg);
		goto done;
	}
	if ((r = sshkey_from_blob(pkblob, blen, &key)) != 0) {
		error("%s: key_from_blob: %s", __func__, ssh_err(r));
		goto done;
	}
	if (key == NULL) {
		error("%s: cannot decode key: %s", __func__, pkalg);
		goto done;
	}
	if (key->type != pktype) {
		error("%s: type mismatch for decoded key "
		    "(received %d, expected %d)", __func__, key->type, pktype);
		goto done;
	}
	if (sshkey_type_plain(key->type) == KEY_RSA &&
	    (ssh->compat & SSH_BUG_RSASIGMD5) != 0) {
		error("Refusing RSA key because peer uses unsafe "
		    "signature format");
		goto done;
	}
	if (match_pattern_list(pkalg, options.hostbased_key_types, 0) != 1) {
		logit("%s: key type %s not in HostbasedAcceptedKeyTypes",
		    __func__, sshkey_type(key));
 		goto done;
 	}
 
	if (!authctxt->valid || authctxt->user == NULL) {
		debug2("%s: disabled because of invalid user", __func__);
		goto done;
	}

 	if ((b = sshbuf_new()) == NULL)
 		fatal("%s: sshbuf_new failed", __func__);
 	/* reconstruct packet */
	if ((r = sshbuf_put_string(b, session_id2, session_id2_len)) != 0 ||
	    (r = sshbuf_put_u8(b, SSH2_MSG_USERAUTH_REQUEST)) != 0 ||
	    (r = sshbuf_put_cstring(b, authctxt->user)) != 0 ||
	    (r = sshbuf_put_cstring(b, authctxt->service)) != 0 ||
	    (r = sshbuf_put_cstring(b, "hostbased")) != 0 ||
	    (r = sshbuf_put_string(b, pkalg, alen)) != 0 ||
	    (r = sshbuf_put_string(b, pkblob, blen)) != 0 ||
	    (r = sshbuf_put_cstring(b, chost)) != 0 ||
	    (r = sshbuf_put_cstring(b, cuser)) != 0)
		fatal("%s: buffer error: %s", __func__, ssh_err(r));
#ifdef DEBUG_PK
	sshbuf_dump(b, stderr);
#endif

	auth2_record_info(authctxt,
	    "client user \"%.100s\", client host \"%.100s\"", cuser, chost);

	/* test for allowed key and correct signature */
	authenticated = 0;
	if (PRIVSEP(hostbased_key_allowed(authctxt->pw, cuser, chost, key)) &&
	    PRIVSEP(sshkey_verify(key, sig, slen,
	    sshbuf_ptr(b), sshbuf_len(b), pkalg, ssh->compat)) == 0)
		authenticated = 1;

	auth2_record_key(authctxt, authenticated, key);
	sshbuf_free(b);
done:
	debug2("%s: authenticated %d", __func__, authenticated);
	sshkey_free(key);
	free(pkalg);
	free(pkblob);
	free(cuser);
	free(chost);
	free(sig);
	return authenticated;
}
