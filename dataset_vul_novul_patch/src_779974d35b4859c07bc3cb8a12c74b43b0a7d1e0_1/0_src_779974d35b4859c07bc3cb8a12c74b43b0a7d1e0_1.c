userauth_pubkey(struct ssh *ssh)
 {
 	Authctxt *authctxt = ssh->authctxt;
 	struct passwd *pw = authctxt->pw;
	struct sshbuf *b = NULL;
 	struct sshkey *key = NULL;
	char *pkalg = NULL, *userstyle = NULL, *key_s = NULL, *ca_s = NULL;
	u_char *pkblob = NULL, *sig = NULL, have_sig;
 	size_t blen, slen;
 	int r, pktype;
 	int authenticated = 0;
 	struct sshauthopt *authopts = NULL;
 
 	if ((r = sshpkt_get_u8(ssh, &have_sig)) != 0 ||
 	    (r = sshpkt_get_cstring(ssh, &pkalg, NULL)) != 0 ||
 	    (r = sshpkt_get_string(ssh, &pkblob, &blen)) != 0)
		fatal("%s: parse request failed: %s", __func__, ssh_err(r));
	pktype = sshkey_type_from_name(pkalg);
	if (pktype == KEY_UNSPEC) {
		/* this is perfectly legal */
		verbose("%s: unsupported public key algorithm: %s",
		    __func__, pkalg);
		goto done;
	}
	if ((r = sshkey_from_blob(pkblob, blen, &key)) != 0) {
		error("%s: could not parse key: %s", __func__, ssh_err(r));
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
		logit("Refusing RSA key because client uses unsafe "
		    "signature scheme");
		goto done;
	}
	if (auth2_key_already_used(authctxt, key)) {
		logit("refusing previously-used %s key", sshkey_type(key));
		goto done;
	}
	if (match_pattern_list(pkalg, options.pubkey_key_types, 0) != 1) {
		logit("%s: key type %s not in PubkeyAcceptedKeyTypes",
		    __func__, sshkey_ssh_name(key));
		goto done;
	}

	key_s = format_key(key);
	if (sshkey_is_cert(key))
		ca_s = format_key(key->cert->signature_key);

	if (have_sig) {
		debug3("%s: have %s signature for %s%s%s",
		    __func__, pkalg, key_s,
		    ca_s == NULL ? "" : " CA ",
		    ca_s == NULL ? "" : ca_s);
		if ((r = sshpkt_get_string(ssh, &sig, &slen)) != 0 ||
		    (r = sshpkt_get_end(ssh)) != 0)
			fatal("%s: %s", __func__, ssh_err(r));
		if ((b = sshbuf_new()) == NULL)
			fatal("%s: sshbuf_new failed", __func__);
		if (ssh->compat & SSH_OLD_SESSIONID) {
			if ((r = sshbuf_put(b, session_id2,
			    session_id2_len)) != 0)
				fatal("%s: sshbuf_put session id: %s",
				    __func__, ssh_err(r));
		} else {
			if ((r = sshbuf_put_string(b, session_id2,
			    session_id2_len)) != 0)
 				fatal("%s: sshbuf_put_string session id: %s",
 				    __func__, ssh_err(r));
 		}
		if (!authctxt->valid || authctxt->user == NULL) {
			debug2("%s: disabled because of invalid user",
			    __func__);
			goto done;
		}
 		/* reconstruct packet */
 		xasprintf(&userstyle, "%s%s%s", authctxt->user,
 		    authctxt->style ? ":" : "",
		    authctxt->style ? authctxt->style : "");
		if ((r = sshbuf_put_u8(b, SSH2_MSG_USERAUTH_REQUEST)) != 0 ||
		    (r = sshbuf_put_cstring(b, userstyle)) != 0 ||
		    (r = sshbuf_put_cstring(b, authctxt->service)) != 0 ||
		    (r = sshbuf_put_cstring(b, "publickey")) != 0 ||
		    (r = sshbuf_put_u8(b, have_sig)) != 0 ||
		    (r = sshbuf_put_cstring(b, pkalg) != 0) ||
		    (r = sshbuf_put_string(b, pkblob, blen)) != 0)
			fatal("%s: build packet failed: %s",
			    __func__, ssh_err(r));
 #ifdef DEBUG_PK
 		sshbuf_dump(b, stderr);
 #endif
 		/* test for correct signature */
 		authenticated = 0;
 		if (PRIVSEP(user_key_allowed(ssh, pw, key, 1, &authopts)) &&
		    PRIVSEP(sshkey_verify(key, sig, slen,
		    sshbuf_ptr(b), sshbuf_len(b),
		    (ssh->compat & SSH_BUG_SIGTYPE) == 0 ? pkalg : NULL,
		    ssh->compat)) == 0) {
 			authenticated = 1;
 		}
 		sshbuf_free(b);
 		auth2_record_key(authctxt, authenticated, key);
 	} else {
 		debug("%s: test pkalg %s pkblob %s%s%s",
		    __func__, pkalg, key_s,
		    ca_s == NULL ? "" : " CA ",
		    ca_s == NULL ? "" : ca_s);

 		if ((r = sshpkt_get_end(ssh)) != 0)
 			fatal("%s: %s", __func__, ssh_err(r));
 
		if (!authctxt->valid || authctxt->user == NULL) {
			debug2("%s: disabled because of invalid user",
			    __func__);
			goto done;
		}
 		/* XXX fake reply and always send PK_OK ? */
 		/*
 		 * XXX this allows testing whether a user is allowed
		 * to login: if you happen to have a valid pubkey this
		 * message is sent. the message is NEVER sent at all
		 * if a user is not allowed to login. is this an
		 * issue? -markus
		 */
		if (PRIVSEP(user_key_allowed(ssh, pw, key, 0, NULL))) {
			if ((r = sshpkt_start(ssh, SSH2_MSG_USERAUTH_PK_OK))
			    != 0 ||
			    (r = sshpkt_put_cstring(ssh, pkalg)) != 0 ||
			    (r = sshpkt_put_string(ssh, pkblob, blen)) != 0 ||
			    (r = sshpkt_send(ssh)) != 0 ||
			    (r = ssh_packet_write_wait(ssh)) != 0)
				fatal("%s: %s", __func__, ssh_err(r));
			authctxt->postponed = 1;
		}
	}
done:
	if (authenticated == 1 && auth_activate_options(ssh, authopts) != 0) {
		debug("%s: key options inconsistent with existing", __func__);
		authenticated = 0;
	}
	debug2("%s: authenticated %d pkalg %s", __func__, authenticated, pkalg);

	sshauthopt_free(authopts);
	sshkey_free(key);
	free(userstyle);
	free(pkalg);
 	free(pkblob);
 	free(key_s);
 	free(ca_s);
	free(sig);
 	return authenticated;
 }
