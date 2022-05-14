static int http_connect(http_subtransport *t)
{
	int error;

	if (t->connected &&
		http_should_keep_alive(&t->parser) &&
		t->parse_finished)
		return 0;

	if (t->io) {
		git_stream_close(t->io);
		git_stream_free(t->io);
		t->io = NULL;
		t->connected = 0;
	}

	if (t->connection_data.use_ssl) {
		error = git_tls_stream_new(&t->io, t->connection_data.host, t->connection_data.port);
	} else {
#ifdef GIT_CURL
		error = git_curl_stream_new(&t->io, t->connection_data.host, t->connection_data.port);
#else
		error = git_socket_stream_new(&t->io,  t->connection_data.host, t->connection_data.port);
#endif
	}

	if (error < 0)
		return error;

	GITERR_CHECK_VERSION(t->io, GIT_STREAM_VERSION, "git_stream");

	apply_proxy_config(t);

	error = git_stream_connect(t->io);

 	if ((!error || error == GIT_ECERTIFICATE) && t->owner->certificate_check_cb != NULL &&
 	    git_stream_is_encrypted(t->io)) {
 		git_cert *cert;
		int is_valid = (error == GIT_OK);
 
 		if ((error = git_stream_certificate(&cert, t->io)) < 0)
 			return error;
 
 		giterr_clear();
 		error = t->owner->certificate_check_cb(cert, is_valid, t->connection_data.host, t->owner->message_cb_payload);
 
 		if (error < 0) {
			if (!giterr_last())
				giterr_set(GITERR_NET, "user cancelled certificate check");

			return error;
		}
	}

	if (error < 0)
		return error;

	t->connected = 1;
	return 0;
}
