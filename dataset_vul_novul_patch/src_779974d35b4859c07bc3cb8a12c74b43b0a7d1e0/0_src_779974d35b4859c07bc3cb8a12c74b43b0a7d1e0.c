userauth_gssapi(struct ssh *ssh)
{
	Authctxt *authctxt = ssh->authctxt;
	gss_OID_desc goid = {0, NULL};
	Gssctxt *ctxt = NULL;
	int r, present;
	u_int mechs;
	OM_uint32 ms;
 	size_t len;
 	u_char *doid = NULL;
 
 	if ((r = sshpkt_get_u32(ssh, &mechs)) != 0)
 		fatal("%s: %s", __func__, ssh_err(r));
 
	if (mechs == 0) {
		debug("Mechanism negotiation is not supported");
		return (0);
	}

	do {
		mechs--;

		free(doid);

		present = 0;
		if ((r = sshpkt_get_string(ssh, &doid, &len)) != 0)
			fatal("%s: %s", __func__, ssh_err(r));

		if (len > 2 && doid[0] == SSH_GSS_OIDTYPE &&
		    doid[1] == len - 2) {
			goid.elements = doid + 2;
			goid.length   = len - 2;
			ssh_gssapi_test_oid_supported(&ms, &goid, &present);
		} else {
			logit("Badly formed OID received");
		}
	} while (mechs > 0 && !present);

	if (!present) {
		free(doid);
		authctxt->server_caused_failure = 1;
 		return (0);
 	}
 
	if (!authctxt->valid || authctxt->user == NULL) {
		debug2("%s: disabled because of invalid user", __func__);
		free(doid);
		return (0);
	}

 	if (GSS_ERROR(PRIVSEP(ssh_gssapi_server_ctx(&ctxt, &goid)))) {
 		if (ctxt != NULL)
 			ssh_gssapi_delete_ctx(&ctxt);
		free(doid);
		authctxt->server_caused_failure = 1;
		return (0);
	}

	authctxt->methoddata = (void *)ctxt;

	/* Return the OID that we received */
	if ((r = sshpkt_start(ssh, SSH2_MSG_USERAUTH_GSSAPI_RESPONSE)) != 0 ||
	    (r = sshpkt_put_string(ssh, doid, len)) != 0 ||
	    (r = sshpkt_send(ssh)) != 0)
		fatal("%s: %s", __func__, ssh_err(r));

	free(doid);

	ssh_dispatch_set(ssh, SSH2_MSG_USERAUTH_GSSAPI_TOKEN, &input_gssapi_token);
	ssh_dispatch_set(ssh, SSH2_MSG_USERAUTH_GSSAPI_ERRTOK, &input_gssapi_errtok);
	authctxt->postponed = 1;

	return (0);
}
