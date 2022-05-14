server_input_global_request(int type, u_int32_t seq, void *ctxt)
{
	char *rtype;
	int want_reply;
	int r, success = 0, allocated_listen_port = 0;
	struct sshbuf *resp = NULL;

	rtype = packet_get_string(NULL);
	want_reply = packet_get_char();
	debug("server_input_global_request: rtype %s want_reply %d", rtype, want_reply);

	/* -R style forwarding */
	if (strcmp(rtype, "tcpip-forward") == 0) {
		struct passwd *pw;
		struct Forward fwd;

		pw = the_authctxt->pw;
		if (pw == NULL || !the_authctxt->valid)
			fatal("server_input_global_request: no/invalid user");
		memset(&fwd, 0, sizeof(fwd));
		fwd.listen_host = packet_get_string(NULL);
		fwd.listen_port = (u_short)packet_get_int();
		debug("server_input_global_request: tcpip-forward listen %s port %d",
		    fwd.listen_host, fwd.listen_port);

		/* check permissions */
		if ((options.allow_tcp_forwarding & FORWARD_REMOTE) == 0 ||
		    no_port_forwarding_flag || options.disable_forwarding ||
		    (!want_reply && fwd.listen_port == 0) ||
		    (fwd.listen_port != 0 &&
		     !bind_permitted(fwd.listen_port, pw->pw_uid))) {
			success = 0;
			packet_send_debug("Server has disabled port forwarding.");
		} else {
			/* Start listening on the port */
			success = channel_setup_remote_fwd_listener(&fwd,
			    &allocated_listen_port, &options.fwd_opts);
		}
		free(fwd.listen_host);
		if ((resp = sshbuf_new()) == NULL)
			fatal("%s: sshbuf_new", __func__);
		if (allocated_listen_port != 0 &&
		    (r = sshbuf_put_u32(resp, allocated_listen_port)) != 0)
			fatal("%s: sshbuf_put_u32: %s", __func__, ssh_err(r));
	} else if (strcmp(rtype, "cancel-tcpip-forward") == 0) {
		struct Forward fwd;

		memset(&fwd, 0, sizeof(fwd));
		fwd.listen_host = packet_get_string(NULL);
		fwd.listen_port = (u_short)packet_get_int();
		debug("%s: cancel-tcpip-forward addr %s port %d", __func__,
		    fwd.listen_host, fwd.listen_port);

		success = channel_cancel_rport_listener(&fwd);
		free(fwd.listen_host);
	} else if (strcmp(rtype, "streamlocal-forward@openssh.com") == 0) {
		struct Forward fwd;

		memset(&fwd, 0, sizeof(fwd));
		fwd.listen_path = packet_get_string(NULL);
		debug("server_input_global_request: streamlocal-forward listen path %s",
		    fwd.listen_path);
 
 		/* check permissions */
 		if ((options.allow_streamlocal_forwarding & FORWARD_REMOTE) == 0
		    || no_port_forwarding_flag || options.disable_forwarding ||
		    !use_privsep) {
 			success = 0;
 			packet_send_debug("Server has disabled port forwarding.");
 		} else {
			/* Start listening on the socket */
			success = channel_setup_remote_fwd_listener(
			    &fwd, NULL, &options.fwd_opts);
		}
		free(fwd.listen_path);
	} else if (strcmp(rtype, "cancel-streamlocal-forward@openssh.com") == 0) {
		struct Forward fwd;

		memset(&fwd, 0, sizeof(fwd));
		fwd.listen_path = packet_get_string(NULL);
		debug("%s: cancel-streamlocal-forward path %s", __func__,
		    fwd.listen_path);

		success = channel_cancel_rport_listener(&fwd);
		free(fwd.listen_path);
	} else if (strcmp(rtype, "no-more-sessions@openssh.com") == 0) {
		no_more_sessions = 1;
		success = 1;
	} else if (strcmp(rtype, "hostkeys-prove-00@openssh.com") == 0) {
		success = server_input_hostkeys_prove(&resp);
	}
	if (want_reply) {
		packet_start(success ?
		    SSH2_MSG_REQUEST_SUCCESS : SSH2_MSG_REQUEST_FAILURE);
		if (success && resp != NULL)
			ssh_packet_put_raw(active_state, sshbuf_ptr(resp),
			    sshbuf_len(resp));
		packet_send();
		packet_write_wait();
	}
	free(rtype);
	sshbuf_free(resp);
	return 0;
}
