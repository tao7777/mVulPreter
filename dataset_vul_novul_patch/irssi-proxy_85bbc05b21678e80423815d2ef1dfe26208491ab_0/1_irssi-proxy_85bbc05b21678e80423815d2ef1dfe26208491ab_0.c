int irssi_ssl_handshake(GIOChannel *handle)
{
	GIOSSLChannel *chan = (GIOSSLChannel *)handle;
	int ret, err;
	X509 *cert;
	const char *errstr;

	ret = SSL_connect(chan->ssl);
	if (ret <= 0) {
		err = SSL_get_error(chan->ssl, ret);
		switch (err) {
			case SSL_ERROR_WANT_READ:
				return 1;
			case SSL_ERROR_WANT_WRITE:
				return 3;
			case SSL_ERROR_ZERO_RETURN:
				g_warning("SSL handshake failed: %s", "server closed connection");
				return -1;
			case SSL_ERROR_SYSCALL:
				errstr = ERR_reason_error_string(ERR_get_error());
				if (errstr == NULL && ret == -1)
					errstr = strerror(errno);
				g_warning("SSL handshake failed: %s", errstr != NULL ? errstr : "server closed connection unexpectedly");
				return -1;
			default:
				errstr = ERR_reason_error_string(ERR_get_error());
				g_warning("SSL handshake failed: %s", errstr != NULL ? errstr : "unknown SSL error");
				return -1;
		}
	}

	cert = SSL_get_peer_certificate(chan->ssl);
	if (cert == NULL) {
 		g_warning("SSL server supplied no certificate");
 		return -1;
 	}
	ret = !chan->verify || irssi_ssl_verify(chan->ssl, chan->ctx, cert);
 	X509_free(cert);
 	return ret ? 0 : -1;
 }
