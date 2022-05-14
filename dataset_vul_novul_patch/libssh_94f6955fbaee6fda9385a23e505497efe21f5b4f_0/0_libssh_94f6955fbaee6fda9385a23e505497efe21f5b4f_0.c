SSH_PACKET_CALLBACK(ssh_packet_newkeys){
  ssh_string sig_blob = NULL;
  int rc;
  (void)packet;
   (void)user;
   (void)type;
   SSH_LOG(SSH_LOG_PROTOCOL, "Received SSH_MSG_NEWKEYS");

  if (session->session_state != SSH_SESSION_STATE_DH ||
      session->dh_handshake_state != DH_STATE_NEWKEYS_SENT) {
      ssh_set_error(session,
                    SSH_FATAL,
                    "ssh_packet_newkeys called in wrong state : %d:%d",
                    session->session_state,session->dh_handshake_state);
      goto error;
   }

   if(session->server){
     /* server things are done in server.c */
     session->dh_handshake_state=DH_STATE_FINISHED;
    if (rc != SSH_OK) {
      goto error;
    }

    /*
     * Set the cryptographic functions for the next crypto
     * (it is needed for generate_session_keys for key lengths)
     */
    if (crypt_set_algorithms(session, SSH_3DES) /* knows nothing about DES*/ ) {
      goto error;
    }

    if (generate_session_keys(session) < 0) {
      goto error;
    }

    /* Verify the host's signature. FIXME do it sooner */
    sig_blob = session->next_crypto->dh_server_signature;
    session->next_crypto->dh_server_signature = NULL;

    /* get the server public key */
    rc = ssh_pki_import_pubkey_blob(session->next_crypto->server_pubkey, &key);
    if (rc < 0) {
        return SSH_ERROR;
    }

    /* check if public key from server matches user preferences */
    if (session->opts.wanted_methods[SSH_HOSTKEYS]) {
        if(!ssh_match_group(session->opts.wanted_methods[SSH_HOSTKEYS],
                            key->type_c)) {
            ssh_set_error(session,
                          SSH_FATAL,
                          "Public key from server (%s) doesn't match user "
                          "preference (%s)",
                          key->type_c,
                          session->opts.wanted_methods[SSH_HOSTKEYS]);
            ssh_key_free(key);
            return -1;
        }
    }

    rc = ssh_pki_signature_verify_blob(session,
                                       sig_blob,
                                       key,
                                       session->next_crypto->secret_hash,
                                       session->next_crypto->digest_len);
    /* Set the server public key type for known host checking */
    session->next_crypto->server_pubkey_type = key->type_c;

    ssh_key_free(key);
    ssh_string_burn(sig_blob);
    ssh_string_free(sig_blob);
    sig_blob = NULL;
    if (rc == SSH_ERROR) {
      goto error;
    }
    SSH_LOG(SSH_LOG_PROTOCOL,"Signature verified and valid");

    /*
     * Once we got SSH2_MSG_NEWKEYS we can switch next_crypto and
     * current_crypto
     */
    if (session->current_crypto) {
      crypto_free(session->current_crypto);
      session->current_crypto=NULL;
    }

    /* FIXME later, include a function to change keys */
    session->current_crypto = session->next_crypto;

    session->next_crypto = crypto_new();
    if (session->next_crypto == NULL) {
      ssh_set_error_oom(session);
      goto error;
    }
    session->next_crypto->session_id = malloc(session->current_crypto->digest_len);
    if (session->next_crypto->session_id == NULL) {
      ssh_set_error_oom(session);
      goto error;
    }
    memcpy(session->next_crypto->session_id, session->current_crypto->session_id,
            session->current_crypto->digest_len);
  }
  session->dh_handshake_state = DH_STATE_FINISHED;
  session->ssh_connection_callback(session);
	return SSH_PACKET_USED;
error:
	session->session_state=SSH_SESSION_STATE_ERROR;
	return SSH_PACKET_USED;
}
