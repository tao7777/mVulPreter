  SSLContext() {
    crypto::EnsureOpenSSLInit();
    ssl_socket_data_index_ = SSL_get_ex_new_index(0, 0, 0, 0, 0);
     DCHECK_NE(ssl_socket_data_index_, -1);
     ssl_ctx_.reset(SSL_CTX_new(SSLv23_client_method()));
     session_cache_.Reset(ssl_ctx_.get(), kDefaultSessionCacheConfig);
    SSL_CTX_set_cert_verify_callback(ssl_ctx_.get(), CertVerifyCallback, NULL);
     SSL_CTX_set_client_cert_cb(ssl_ctx_.get(), ClientCertCallback);
     SSL_CTX_set_channel_id_cb(ssl_ctx_.get(), ChannelIDCallback);
    SSL_CTX_set_verify(ssl_ctx_.get(), SSL_VERIFY_PEER, NULL);
 #if defined(OPENSSL_NPN_NEGOTIATED)
    SSL_CTX_set_next_proto_select_cb(ssl_ctx_.get(), SelectNextProtoCallback,
                                     NULL);
#endif
  }
