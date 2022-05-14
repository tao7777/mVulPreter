static GIOChannel *irssi_ssl_get_iochannel(GIOChannel *handle, const char *mycert, const char *mypkey, const char *cafile, const char *capath, gboolean verify)
 {
 	GIOSSLChannel *chan;
 	GIOChannel *gchan;
	int fd;
	SSL *ssl;
	SSL_CTX *ctx = NULL;

	g_return_val_if_fail(handle != NULL, NULL);

	if(!ssl_ctx && !irssi_ssl_init())
		return NULL;

	if(!(fd = g_io_channel_unix_get_fd(handle)))
		return NULL;

	if (mycert && *mycert) {
		char *scert = NULL, *spkey = NULL;
		if ((ctx = SSL_CTX_new(SSLv23_client_method())) == NULL) {
			g_error("Could not allocate memory for SSL context");
			return NULL;
		}
		scert = convert_home(mycert);
		if (mypkey && *mypkey)
			spkey = convert_home(mypkey);
		if (! SSL_CTX_use_certificate_file(ctx, scert, SSL_FILETYPE_PEM))
			g_warning("Loading of client certificate '%s' failed", mycert);
		else if (! SSL_CTX_use_PrivateKey_file(ctx, spkey ? spkey : scert, SSL_FILETYPE_PEM))
			g_warning("Loading of private key '%s' failed", mypkey ? mypkey : mycert);
		else if (! SSL_CTX_check_private_key(ctx))
			g_warning("Private key does not match the certificate");
		g_free(scert);
		g_free(spkey);
	}

	if ((cafile && *cafile) || (capath && *capath)) {
		char *scafile = NULL;
		char *scapath = NULL;
		if (! ctx && (ctx = SSL_CTX_new(SSLv23_client_method())) == NULL) {
			g_error("Could not allocate memory for SSL context");
			return NULL;
		}
		if (cafile && *cafile)
			scafile = convert_home(cafile);
		if (capath && *capath)
			scapath = convert_home(capath);
		if (! SSL_CTX_load_verify_locations(ctx, scafile, scapath)) {
			g_warning("Could not load CA list for verifying SSL server certificate");
			g_free(scafile);
			g_free(scapath);
			SSL_CTX_free(ctx);
			return NULL;
		}
		g_free(scafile);
		g_free(scapath);
		verify = TRUE;
	}

	if (ctx == NULL)
		ctx = ssl_ctx;

	if(!(ssl = SSL_new(ctx)))
	{
		g_warning("Failed to allocate SSL structure");
		return NULL;
	}

	if(!SSL_set_fd(ssl, fd))
	{
		g_warning("Failed to associate socket to SSL stream");
		SSL_free(ssl);
		if (ctx != ssl_ctx)
			SSL_CTX_free(ctx);
		return NULL;
	}

	SSL_set_mode(ssl, SSL_MODE_ENABLE_PARTIAL_WRITE |
			SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);

	chan = g_new0(GIOSSLChannel, 1);
	chan->fd = fd;
	chan->giochan = handle;
 	chan->ssl = ssl;
 	chan->ctx = ctx;
 	chan->verify = verify;
 
 	gchan = (GIOChannel *)chan;
 	gchan->funcs = &irssi_ssl_channel_funcs;
	g_io_channel_init(gchan);
	gchan->is_readable = gchan->is_writeable = TRUE;
	gchan->use_buffer = FALSE;

 	return gchan;
 }
