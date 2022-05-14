static void ssh_throttle_all(Ssh ssh, int enable, int bufsize)
{
    int i;
    struct ssh_channel *c;

    if (enable == ssh->throttled_all)
	return;
    ssh->throttled_all = enable;
    ssh->overall_bufsize = bufsize;
    if (!ssh->channels)
	return;
    for (i = 0; NULL != (c = index234(ssh->channels, i)); i++) {
	switch (c->type) {
	  case CHAN_MAINSESSION:
	    /*
	     * This is treated separately, outside the switch.
	     */
	    break;
            x11_override_throttle(c->u.x11.xconn, enable);
            break;
          case CHAN_AGENT:
           /* Agent forwarding channels are buffer-managed by
             * checking ssh->throttled_all in ssh_agentf_try_forward.
             * So at the moment we _un_throttle again, we must make an
             * attempt to do something. */
            if (!enable)
                ssh_agentf_try_forward(c);
            break;
          case CHAN_SOCKDATA:
            pfd_override_throttle(c->u.pfd.pf, enable);

static void ssh_agent_callback(void *sshv, void *reply, int replylen)
{
    Ssh ssh = (Ssh) sshv;

    ssh->auth_agent_query = NULL;

    ssh->agent_response = reply;
    ssh->agent_response_len = replylen;

    if (ssh->version == 1)
	do_ssh1_login(ssh, NULL, -1, NULL);
    else
	do_ssh2_authconn(ssh, NULL, -1, NULL);
}

static void ssh_dialog_callback(void *sshv, int ret)
{
    Ssh ssh = (Ssh) sshv;

    ssh->user_response = ret;

    if (ssh->version == 1)
	do_ssh1_login(ssh, NULL, -1, NULL);
    else
	do_ssh2_transport(ssh, NULL, -1, NULL);

    /*
     * This may have unfrozen the SSH connection, so do a
     * queued-data run.
     */
    ssh_process_queued_incoming_data(ssh);
}

static void ssh_agentf_callback(void *cv, void *reply, int replylen)
     ssh_process_queued_incoming_data(ssh);
 }
 
static void ssh_agentf_got_response(struct ssh_channel *c,
                                    void *reply, int replylen)
 {
     c->u.a.pending = NULL;

    if (!reply) {
       /* The real agent didn't send any kind of reply at all for
         * some reason, so fake an SSH_AGENT_FAILURE. */
       reply = "\0\0\0\1\5";
        replylen = 5;
     }

    ssh_send_channel_data(c, reply, replylen);
}

static void ssh_agentf_callback(void *cv, void *reply, int replylen);

static void ssh_agentf_try_forward(struct ssh_channel *c)
{
    unsigned datalen, lengthfield, messagelen;
    unsigned char *message;
    unsigned char msglen[4];
    void *reply;
    int replylen;

     /*
     * Don't try to parallelise agent requests. Wait for each one to
     * return before attempting the next.
      */
    if (c->u.a.pending)
        return;

    /*
     * If the outgoing side of the channel connection is currently
     * throttled (for any reason, either that channel's window size or
     * the entire SSH connection being throttled), don't submit any
     * new forwarded requests to the real agent. This causes the input
     * side of the agent forwarding not to be emptied, exerting the
     * required back-pressure on the remote client, and encouraging it
     * to read our responses before sending too many more requests.
     */
    if (c->ssh->throttled_all ||
        (c->ssh->version == 2 && c->v.v2.remwindow == 0))
        return;

    while (1) {
        /*
         * Try to extract a complete message from the input buffer.
         */
        datalen = bufchain_size(&c->u.a.inbuffer);
        if (datalen < 4)
            break;         /* not even a length field available yet */

        bufchain_fetch(&c->u.a.inbuffer, msglen, 4);
        lengthfield = GET_32BIT(msglen);
        if (lengthfield > datalen - 4)
            break;          /* a whole message is not yet available */

        messagelen = lengthfield + 4;

        message = snewn(messagelen, unsigned char);
        bufchain_fetch(&c->u.a.inbuffer, message, messagelen);
        bufchain_consume(&c->u.a.inbuffer, messagelen);
        c->u.a.pending = agent_query(
            message, messagelen, &reply, &replylen, ssh_agentf_callback, c);
        sfree(message);

        if (c->u.a.pending)
            return;   /* agent_query promised to reply in due course */

        /*
         * If the agent gave us an answer immediately, pass it
         * straight on and go round this loop again.
         */
        ssh_agentf_got_response(c, reply, replylen);
    }

    /*
     * If we get here (i.e. we left the above while loop via 'break'
     * rather than 'return'), that means we've determined that the
     * input buffer for the agent forwarding connection doesn't
     * contain a complete request.
     *
     * So if there's potentially more data to come, we can return now,
     * and wait for the remote client to send it. But if the remote
     * has sent EOF, it would be a mistake to do that, because we'd be
     * waiting a long time. So this is the moment to check for EOF,
     * and respond appropriately.
     */
    if (c->closes & CLOSES_RCVD_EOF)
         sshfwd_write_eof(c);
 }
 
static void ssh_agentf_callback(void *cv, void *reply, int replylen)
{
    struct ssh_channel *c = (struct ssh_channel *)cv;

    ssh_agentf_got_response(c, reply, replylen);
    sfree(reply);

    /*
     * Now try to extract and send further messages from the channel's
     * input-side buffer.
     */
    ssh_agentf_try_forward(c);
}

 /*
  * Client-initiated disconnection. Send a DISCONNECT if `wire_reason'
  * non-NULL, otherwise just close the connection. `client_reason' == NULL
			 struct Packet *pktin)
{
    int i, j, ret;
    unsigned char cookie[8], *ptr;
    struct MD5Context md5c;
    struct do_ssh1_login_state {
	int crLine;
	int len;
	unsigned char *rsabuf;
        const unsigned char *keystr1, *keystr2;
	unsigned long supported_ciphers_mask, supported_auths_mask;
	int tried_publickey, tried_agent;
	int tis_auth_refused, ccard_auth_refused;
	unsigned char session_id[16];
	int cipher_type;
	void *publickey_blob;
	int publickey_bloblen;
	char *publickey_comment;
	int privatekey_available, privatekey_encrypted;
	prompts_t *cur_prompt;
	char c;
	int pwpkt_type;
	unsigned char request[5], *response, *p;
	int responselen;
	int keyi, nkeys;
	int authed;
	struct RSAKey key;
	Bignum challenge;
	char *commentp;
	int commentlen;
        int dlgret;
	Filename *keyfile;
        struct RSAKey servkey, hostkey;
    };
    crState(do_ssh1_login_state);

    crBeginState;

    if (!pktin)
	crWaitUntil(pktin);

    if (pktin->type != SSH1_SMSG_PUBLIC_KEY) {
	bombout(("Public key packet not received"));
	crStop(0);
    }

    logevent("Received public keys");

    ptr = ssh_pkt_getdata(pktin, 8);
    if (!ptr) {
	bombout(("SSH-1 public key packet stopped before random cookie"));
	crStop(0);
    }
    memcpy(cookie, ptr, 8);

    if (!ssh1_pkt_getrsakey(pktin, &s->servkey, &s->keystr1) ||
	!ssh1_pkt_getrsakey(pktin, &s->hostkey, &s->keystr2)) {	
	bombout(("Failed to read SSH-1 public keys from public key packet"));
	crStop(0);
    }

    /*
     * Log the host key fingerprint.
     */
    {
	char logmsg[80];
	logevent("Host key fingerprint is:");
	strcpy(logmsg, "      ");
	s->hostkey.comment = NULL;
	rsa_fingerprint(logmsg + strlen(logmsg),
			sizeof(logmsg) - strlen(logmsg), &s->hostkey);
	logevent(logmsg);
    }

    ssh->v1_remote_protoflags = ssh_pkt_getuint32(pktin);
    s->supported_ciphers_mask = ssh_pkt_getuint32(pktin);
    s->supported_auths_mask = ssh_pkt_getuint32(pktin);
    if ((ssh->remote_bugs & BUG_CHOKES_ON_RSA))
	s->supported_auths_mask &= ~(1 << SSH1_AUTH_RSA);

    ssh->v1_local_protoflags =
	ssh->v1_remote_protoflags & SSH1_PROTOFLAGS_SUPPORTED;
    ssh->v1_local_protoflags |= SSH1_PROTOFLAG_SCREEN_NUMBER;

    MD5Init(&md5c);
    MD5Update(&md5c, s->keystr2, s->hostkey.bytes);
    MD5Update(&md5c, s->keystr1, s->servkey.bytes);
    MD5Update(&md5c, cookie, 8);
    MD5Final(s->session_id, &md5c);

    for (i = 0; i < 32; i++)
	ssh->session_key[i] = random_byte();

    /*
     * Verify that the `bits' and `bytes' parameters match.
     */
    if (s->hostkey.bits > s->hostkey.bytes * 8 ||
	s->servkey.bits > s->servkey.bytes * 8) {
	bombout(("SSH-1 public keys were badly formatted"));
	crStop(0);
    }

    s->len = (s->hostkey.bytes > s->servkey.bytes ?
              s->hostkey.bytes : s->servkey.bytes);

    s->rsabuf = snewn(s->len, unsigned char);

    /*
     * Verify the host key.
     */
    {
	/*
	 * First format the key into a string.
	 */
	int len = rsastr_len(&s->hostkey);
	char fingerprint[100];
	char *keystr = snewn(len, char);
	rsastr_fmt(keystr, &s->hostkey);
	rsa_fingerprint(fingerprint, sizeof(fingerprint), &s->hostkey);

        /* First check against manually configured host keys. */
        s->dlgret = verify_ssh_manual_host_key(ssh, fingerprint, NULL, NULL);
        if (s->dlgret == 0) {          /* did not match */
            bombout(("Host key did not appear in manually configured list"));
            sfree(keystr);
            crStop(0);
        } else if (s->dlgret < 0) { /* none configured; use standard handling */
            ssh_set_frozen(ssh, 1);
            s->dlgret = verify_ssh_host_key(ssh->frontend,
                                            ssh->savedhost, ssh->savedport,
                                            "rsa", keystr, fingerprint,
                                            ssh_dialog_callback, ssh);
            sfree(keystr);
#ifdef FUZZING
	    s->dlgret = 1;
#endif
            if (s->dlgret < 0) {
                do {
                    crReturn(0);
                    if (pktin) {
                        bombout(("Unexpected data from server while waiting"
                                 " for user host key response"));
                        crStop(0);
                    }
                } while (pktin || inlen > 0);
                s->dlgret = ssh->user_response;
            }
            ssh_set_frozen(ssh, 0);

            if (s->dlgret == 0) {
                ssh_disconnect(ssh, "User aborted at host key verification",
                               NULL, 0, TRUE);
                crStop(0);
            }
        } else {
            sfree(keystr);
        }
    }

    for (i = 0; i < 32; i++) {
	s->rsabuf[i] = ssh->session_key[i];
	if (i < 16)
	    s->rsabuf[i] ^= s->session_id[i];
    }

    if (s->hostkey.bytes > s->servkey.bytes) {
	ret = rsaencrypt(s->rsabuf, 32, &s->servkey);
	if (ret)
	    ret = rsaencrypt(s->rsabuf, s->servkey.bytes, &s->hostkey);
    } else {
	ret = rsaencrypt(s->rsabuf, 32, &s->hostkey);
	if (ret)
	    ret = rsaencrypt(s->rsabuf, s->hostkey.bytes, &s->servkey);
    }
    if (!ret) {
	bombout(("SSH-1 public key encryptions failed due to bad formatting"));
	crStop(0);	
    }

    logevent("Encrypted session key");

    {
	int cipher_chosen = 0, warn = 0;
	const char *cipher_string = NULL;
	int i;
	for (i = 0; !cipher_chosen && i < CIPHER_MAX; i++) {
	    int next_cipher = conf_get_int_int(ssh->conf,
					       CONF_ssh_cipherlist, i);
	    if (next_cipher == CIPHER_WARN) {
		/* If/when we choose a cipher, warn about it */
		warn = 1;
	    } else if (next_cipher == CIPHER_AES) {
		/* XXX Probably don't need to mention this. */
		logevent("AES not supported in SSH-1, skipping");
	    } else {
		switch (next_cipher) {
		  case CIPHER_3DES:     s->cipher_type = SSH_CIPHER_3DES;
					cipher_string = "3DES"; break;
		  case CIPHER_BLOWFISH: s->cipher_type = SSH_CIPHER_BLOWFISH;
					cipher_string = "Blowfish"; break;
		  case CIPHER_DES:	s->cipher_type = SSH_CIPHER_DES;
					cipher_string = "single-DES"; break;
		}
		if (s->supported_ciphers_mask & (1 << s->cipher_type))
		    cipher_chosen = 1;
	    }
	}
	if (!cipher_chosen) {
	    if ((s->supported_ciphers_mask & (1 << SSH_CIPHER_3DES)) == 0)
		bombout(("Server violates SSH-1 protocol by not "
			 "supporting 3DES encryption"));
	    else
		/* shouldn't happen */
		bombout(("No supported ciphers found"));
	    crStop(0);
	}

	/* Warn about chosen cipher if necessary. */
	if (warn) {
            ssh_set_frozen(ssh, 1);
	    s->dlgret = askalg(ssh->frontend, "cipher", cipher_string,
			       ssh_dialog_callback, ssh);
	    if (s->dlgret < 0) {
		do {
		    crReturn(0);
		    if (pktin) {
			bombout(("Unexpected data from server while waiting"
				 " for user response"));
			crStop(0);
		    }
		} while (pktin || inlen > 0);
		s->dlgret = ssh->user_response;
	    }
            ssh_set_frozen(ssh, 0);
	    if (s->dlgret == 0) {
		ssh_disconnect(ssh, "User aborted at cipher warning", NULL,
			       0, TRUE);
		crStop(0);
	    }
        }
    }

    switch (s->cipher_type) {
      case SSH_CIPHER_3DES:
	logevent("Using 3DES encryption");
	break;
      case SSH_CIPHER_DES:
	logevent("Using single-DES encryption");
	break;
      case SSH_CIPHER_BLOWFISH:
	logevent("Using Blowfish encryption");
	break;
    }

    send_packet(ssh, SSH1_CMSG_SESSION_KEY,
		PKT_CHAR, s->cipher_type,
		PKT_DATA, cookie, 8,
		PKT_CHAR, (s->len * 8) >> 8, PKT_CHAR, (s->len * 8) & 0xFF,
		PKT_DATA, s->rsabuf, s->len,
		PKT_INT, ssh->v1_local_protoflags, PKT_END);

    logevent("Trying to enable encryption...");

    sfree(s->rsabuf);

    ssh->cipher = (s->cipher_type == SSH_CIPHER_BLOWFISH ? &ssh_blowfish_ssh1 :
		   s->cipher_type == SSH_CIPHER_DES ? &ssh_des :
		   &ssh_3des);
    ssh->v1_cipher_ctx = ssh->cipher->make_context();
    ssh->cipher->sesskey(ssh->v1_cipher_ctx, ssh->session_key);
    logeventf(ssh, "Initialised %s encryption", ssh->cipher->text_name);

    ssh->crcda_ctx = crcda_make_context();
    logevent("Installing CRC compensation attack detector");

    if (s->servkey.modulus) {
	sfree(s->servkey.modulus);
	s->servkey.modulus = NULL;
    }
    if (s->servkey.exponent) {
	sfree(s->servkey.exponent);
	s->servkey.exponent = NULL;
    }
    if (s->hostkey.modulus) {
	sfree(s->hostkey.modulus);
	s->hostkey.modulus = NULL;
    }
    if (s->hostkey.exponent) {
	sfree(s->hostkey.exponent);
	s->hostkey.exponent = NULL;
    }
    crWaitUntil(pktin);

    if (pktin->type != SSH1_SMSG_SUCCESS) {
	bombout(("Encryption not successfully enabled"));
	crStop(0);
    }

    logevent("Successfully started encryption");

    fflush(stdout); /* FIXME eh? */
    {
	if ((ssh->username = get_remote_username(ssh->conf)) == NULL) {
	    int ret; /* need not be kept over crReturn */
	    s->cur_prompt = new_prompts(ssh->frontend);
	    s->cur_prompt->to_server = TRUE;
	    s->cur_prompt->name = dupstr("SSH login name");
	    add_prompt(s->cur_prompt, dupstr("login as: "), TRUE);
	    ret = get_userpass_input(s->cur_prompt, NULL, 0);
	    while (ret < 0) {
		ssh->send_ok = 1;
		crWaitUntil(!pktin);
		ret = get_userpass_input(s->cur_prompt, in, inlen);
		ssh->send_ok = 0;
	    }
	    if (!ret) {
		/*
		 * Failed to get a username. Terminate.
		 */
		free_prompts(s->cur_prompt);
		ssh_disconnect(ssh, "No username provided", NULL, 0, TRUE);
		crStop(0);
	    }
	    ssh->username = dupstr(s->cur_prompt->prompts[0]->result);
	    free_prompts(s->cur_prompt);
	}

	send_packet(ssh, SSH1_CMSG_USER, PKT_STR, ssh->username, PKT_END);
	{
	    char *userlog = dupprintf("Sent username \"%s\"", ssh->username);
	    logevent(userlog);
	    if (flags & FLAG_INTERACTIVE &&
		(!((flags & FLAG_STDERR) && (flags & FLAG_VERBOSE)))) {
		c_write_str(ssh, userlog);
		c_write_str(ssh, "\r\n");
	    }
	    sfree(userlog);
	}
    }

    crWaitUntil(pktin);

    if ((s->supported_auths_mask & (1 << SSH1_AUTH_RSA)) == 0) {
	/* We must not attempt PK auth. Pretend we've already tried it. */
	s->tried_publickey = s->tried_agent = 1;
    } else {
	s->tried_publickey = s->tried_agent = 0;
    }
    s->tis_auth_refused = s->ccard_auth_refused = 0;
    /*
     * Load the public half of any configured keyfile for later use.
     */
    s->keyfile = conf_get_filename(ssh->conf, CONF_keyfile);
    if (!filename_is_null(s->keyfile)) {
	int keytype;
	logeventf(ssh, "Reading key file \"%.150s\"",
		  filename_to_str(s->keyfile));
	keytype = key_type(s->keyfile);
	if (keytype == SSH_KEYTYPE_SSH1 ||
            keytype == SSH_KEYTYPE_SSH1_PUBLIC) {
	    const char *error;
	    if (rsakey_pubblob(s->keyfile,
			       &s->publickey_blob, &s->publickey_bloblen,
			       &s->publickey_comment, &error)) {
                s->privatekey_available = (keytype == SSH_KEYTYPE_SSH1);
                if (!s->privatekey_available)
                    logeventf(ssh, "Key file contains public key only");
		s->privatekey_encrypted = rsakey_encrypted(s->keyfile,
                                                           NULL);
	    } else {
		char *msgbuf;
		logeventf(ssh, "Unable to load key (%s)", error);
		msgbuf = dupprintf("Unable to load key file "
				   "\"%.150s\" (%s)\r\n",
				   filename_to_str(s->keyfile),
				   error);
		c_write_str(ssh, msgbuf);
		sfree(msgbuf);
		s->publickey_blob = NULL;
	    }
	} else {
	    char *msgbuf;
	    logeventf(ssh, "Unable to use this key file (%s)",
		      key_type_to_str(keytype));
	    msgbuf = dupprintf("Unable to use key file \"%.150s\""
			       " (%s)\r\n",
			       filename_to_str(s->keyfile),
			       key_type_to_str(keytype));
	    c_write_str(ssh, msgbuf);
	    sfree(msgbuf);
	    s->publickey_blob = NULL;
	}
    } else
	s->publickey_blob = NULL;

    while (pktin->type == SSH1_SMSG_FAILURE) {
	s->pwpkt_type = SSH1_CMSG_AUTH_PASSWORD;

	if (conf_get_int(ssh->conf, CONF_tryagent) && agent_exists() && !s->tried_agent) {
	    /*
	     * Attempt RSA authentication using Pageant.
	     */
	    void *r;

	    s->authed = FALSE;
	    s->tried_agent = 1;
	    logevent("Pageant is running. Requesting keys.");

	    /* Request the keys held by the agent. */
	    PUT_32BIT(s->request, 1);
	    s->request[4] = SSH1_AGENTC_REQUEST_RSA_IDENTITIES;
            ssh->auth_agent_query = agent_query(
                s->request, 5, &r, &s->responselen, ssh_agent_callback, ssh);
	    if (ssh->auth_agent_query) {
		do {
		    crReturn(0);
		    if (pktin) {
			bombout(("Unexpected data from server while waiting"
				 " for agent response"));
			crStop(0);
		    }
		} while (pktin || inlen > 0);
		r = ssh->agent_response;
		s->responselen = ssh->agent_response_len;
	    }
	    s->response = (unsigned char *) r;
	    if (s->response && s->responselen >= 5 &&
		s->response[4] == SSH1_AGENT_RSA_IDENTITIES_ANSWER) {
		s->p = s->response + 5;
		s->nkeys = toint(GET_32BIT(s->p));
                if (s->nkeys < 0) {
                    logeventf(ssh, "Pageant reported negative key count %d",
                              s->nkeys);
                    s->nkeys = 0;
                }
		s->p += 4;
		logeventf(ssh, "Pageant has %d SSH-1 keys", s->nkeys);
		for (s->keyi = 0; s->keyi < s->nkeys; s->keyi++) {
		    unsigned char *pkblob = s->p;
		    s->p += 4;
		    {
			int n, ok = FALSE;
			do {	       /* do while (0) to make breaking easy */
			    n = ssh1_read_bignum
				(s->p, toint(s->responselen-(s->p-s->response)),
				 &s->key.exponent);
			    if (n < 0)
				break;
			    s->p += n;
			    n = ssh1_read_bignum
				(s->p, toint(s->responselen-(s->p-s->response)),
				 &s->key.modulus);
			    if (n < 0)
                                break;
			    s->p += n;
			    if (s->responselen - (s->p-s->response) < 4)
				break;
			    s->commentlen = toint(GET_32BIT(s->p));
			    s->p += 4;
			    if (s->commentlen < 0 ||
                                toint(s->responselen - (s->p-s->response)) <
				s->commentlen)
				break;
			    s->commentp = (char *)s->p;
			    s->p += s->commentlen;
			    ok = TRUE;
			} while (0);
			if (!ok) {
			    logevent("Pageant key list packet was truncated");
			    break;
			}
		    }
		    if (s->publickey_blob) {
			if (!memcmp(pkblob, s->publickey_blob,
				    s->publickey_bloblen)) {
			    logeventf(ssh, "Pageant key #%d matches "
				      "configured key file", s->keyi);
			    s->tried_publickey = 1;
			} else
			    /* Skip non-configured key */
			    continue;
		    }
		    logeventf(ssh, "Trying Pageant key #%d", s->keyi);
		    send_packet(ssh, SSH1_CMSG_AUTH_RSA,
				PKT_BIGNUM, s->key.modulus, PKT_END);
		    crWaitUntil(pktin);
		    if (pktin->type != SSH1_SMSG_AUTH_RSA_CHALLENGE) {
			logevent("Key refused");
			continue;
		    }
		    logevent("Received RSA challenge");
		    if ((s->challenge = ssh1_pkt_getmp(pktin)) == NULL) {
			bombout(("Server's RSA challenge was badly formatted"));
			crStop(0);
		    }

		    {
			char *agentreq, *q, *ret;
			void *vret;
			int len, retlen;
			len = 1 + 4;   /* message type, bit count */
			len += ssh1_bignum_length(s->key.exponent);
			len += ssh1_bignum_length(s->key.modulus);
			len += ssh1_bignum_length(s->challenge);
			len += 16;     /* session id */
			len += 4;      /* response format */
			agentreq = snewn(4 + len, char);
			PUT_32BIT(agentreq, len);
			q = agentreq + 4;
			*q++ = SSH1_AGENTC_RSA_CHALLENGE;
			PUT_32BIT(q, bignum_bitcount(s->key.modulus));
			q += 4;
			q += ssh1_write_bignum(q, s->key.exponent);
			q += ssh1_write_bignum(q, s->key.modulus);
			q += ssh1_write_bignum(q, s->challenge);
			memcpy(q, s->session_id, 16);
			q += 16;
			PUT_32BIT(q, 1);	/* response format */
                        ssh->auth_agent_query = agent_query(
                            agentreq, len + 4, &vret, &retlen,
                            ssh_agent_callback, ssh);
			if (ssh->auth_agent_query) {
			    sfree(agentreq);
			    do {
				crReturn(0);
				if (pktin) {
				    bombout(("Unexpected data from server"
					     " while waiting for agent"
					     " response"));
				    crStop(0);
				}
			    } while (pktin || inlen > 0);
			    vret = ssh->agent_response;
			    retlen = ssh->agent_response_len;
			} else
			    sfree(agentreq);
			ret = vret;
			if (ret) {
			    if (ret[4] == SSH1_AGENT_RSA_RESPONSE) {
				logevent("Sending Pageant's response");
				send_packet(ssh, SSH1_CMSG_AUTH_RSA_RESPONSE,
					    PKT_DATA, ret + 5, 16,
					    PKT_END);
				sfree(ret);
				crWaitUntil(pktin);
				if (pktin->type == SSH1_SMSG_SUCCESS) {
				    logevent
					("Pageant's response accepted");
				    if (flags & FLAG_VERBOSE) {
					c_write_str(ssh, "Authenticated using"
						    " RSA key \"");
					c_write(ssh, s->commentp,
						s->commentlen);
					c_write_str(ssh, "\" from agent\r\n");
				    }
				    s->authed = TRUE;
				} else
				    logevent
					("Pageant's response not accepted");
			    } else {
				logevent
				    ("Pageant failed to answer challenge");
				sfree(ret);
			    }
			} else {
			    logevent("No reply received from Pageant");
			}
		    }
		    freebn(s->key.exponent);
		    freebn(s->key.modulus);
		    freebn(s->challenge);
		    if (s->authed)
			break;
		}
		sfree(s->response);
		if (s->publickey_blob && !s->tried_publickey)
		    logevent("Configured key file not in Pageant");
	    } else {
                logevent("Failed to get reply from Pageant");
            }
	    if (s->authed)
		break;
	}
	if (s->publickey_blob && s->privatekey_available &&
            !s->tried_publickey) {
	    /*
	     * Try public key authentication with the specified
	     * key file.
	     */
	    int got_passphrase; /* need not be kept over crReturn */
	    if (flags & FLAG_VERBOSE)
		c_write_str(ssh, "Trying public key authentication.\r\n");
	    s->keyfile = conf_get_filename(ssh->conf, CONF_keyfile);
	    logeventf(ssh, "Trying public key \"%s\"",
		      filename_to_str(s->keyfile));
	    s->tried_publickey = 1;
	    got_passphrase = FALSE;
	    while (!got_passphrase) {
		/*
		 * Get a passphrase, if necessary.
		 */
		char *passphrase = NULL;    /* only written after crReturn */
		const char *error;
		if (!s->privatekey_encrypted) {
		    if (flags & FLAG_VERBOSE)
			c_write_str(ssh, "No passphrase required.\r\n");
		    passphrase = NULL;
		} else {
		    int ret; /* need not be kept over crReturn */
		    s->cur_prompt = new_prompts(ssh->frontend);
		    s->cur_prompt->to_server = FALSE;
		    s->cur_prompt->name = dupstr("SSH key passphrase");
		    add_prompt(s->cur_prompt,
			       dupprintf("Passphrase for key \"%.100s\": ",
					 s->publickey_comment), FALSE);
		    ret = get_userpass_input(s->cur_prompt, NULL, 0);
		    while (ret < 0) {
			ssh->send_ok = 1;
			crWaitUntil(!pktin);
			ret = get_userpass_input(s->cur_prompt, in, inlen);
			ssh->send_ok = 0;
		    }
		    if (!ret) {
			/* Failed to get a passphrase. Terminate. */
			free_prompts(s->cur_prompt);
			ssh_disconnect(ssh, NULL, "Unable to authenticate",
				       0, TRUE);
			crStop(0);
		    }
		    passphrase = dupstr(s->cur_prompt->prompts[0]->result);
		    free_prompts(s->cur_prompt);
		}
		/*
		 * Try decrypting key with passphrase.
		 */
		s->keyfile = conf_get_filename(ssh->conf, CONF_keyfile);
		ret = loadrsakey(s->keyfile, &s->key, passphrase,
				 &error);
		if (passphrase) {
		    smemclr(passphrase, strlen(passphrase));
		    sfree(passphrase);
		}
		if (ret == 1) {
		    /* Correct passphrase. */
		    got_passphrase = TRUE;
		} else if (ret == 0) {
		    c_write_str(ssh, "Couldn't load private key from ");
		    c_write_str(ssh, filename_to_str(s->keyfile));
		    c_write_str(ssh, " (");
		    c_write_str(ssh, error);
		    c_write_str(ssh, ").\r\n");
		    got_passphrase = FALSE;
		    break;	       /* go and try something else */
		} else if (ret == -1) {
		    c_write_str(ssh, "Wrong passphrase.\r\n"); /* FIXME */
		    got_passphrase = FALSE;
		    /* and try again */
		} else {
		    assert(0 && "unexpected return from loadrsakey()");
		    got_passphrase = FALSE;   /* placate optimisers */
		}
	    }

	    if (got_passphrase) {

		/*
		 * Send a public key attempt.
		 */
		send_packet(ssh, SSH1_CMSG_AUTH_RSA,
			    PKT_BIGNUM, s->key.modulus, PKT_END);

		crWaitUntil(pktin);
		if (pktin->type == SSH1_SMSG_FAILURE) {
		    c_write_str(ssh, "Server refused our public key.\r\n");
		    continue;	       /* go and try something else */
		}
		if (pktin->type != SSH1_SMSG_AUTH_RSA_CHALLENGE) {
		    bombout(("Bizarre response to offer of public key"));
		    crStop(0);
		}

		{
		    int i;
		    unsigned char buffer[32];
		    Bignum challenge, response;

		    if ((challenge = ssh1_pkt_getmp(pktin)) == NULL) {
			bombout(("Server's RSA challenge was badly formatted"));
			crStop(0);
		    }
		    response = rsadecrypt(challenge, &s->key);
		    freebn(s->key.private_exponent);/* burn the evidence */

		    for (i = 0; i < 32; i++) {
			buffer[i] = bignum_byte(response, 31 - i);
		    }

		    MD5Init(&md5c);
		    MD5Update(&md5c, buffer, 32);
		    MD5Update(&md5c, s->session_id, 16);
		    MD5Final(buffer, &md5c);

		    send_packet(ssh, SSH1_CMSG_AUTH_RSA_RESPONSE,
				PKT_DATA, buffer, 16, PKT_END);

		    freebn(challenge);
		    freebn(response);
		}

		crWaitUntil(pktin);
		if (pktin->type == SSH1_SMSG_FAILURE) {
		    if (flags & FLAG_VERBOSE)
			c_write_str(ssh, "Failed to authenticate with"
				    " our public key.\r\n");
		    continue;	       /* go and try something else */
		} else if (pktin->type != SSH1_SMSG_SUCCESS) {
		    bombout(("Bizarre response to RSA authentication response"));
		    crStop(0);
		}

		break;		       /* we're through! */
	    }

	}

	/*
	 * Otherwise, try various forms of password-like authentication.
	 */
	s->cur_prompt = new_prompts(ssh->frontend);

	if (conf_get_int(ssh->conf, CONF_try_tis_auth) &&
	    (s->supported_auths_mask & (1 << SSH1_AUTH_TIS)) &&
	    !s->tis_auth_refused) {
	    s->pwpkt_type = SSH1_CMSG_AUTH_TIS_RESPONSE;
	    logevent("Requested TIS authentication");
	    send_packet(ssh, SSH1_CMSG_AUTH_TIS, PKT_END);
	    crWaitUntil(pktin);
	    if (pktin->type != SSH1_SMSG_AUTH_TIS_CHALLENGE) {
		logevent("TIS authentication declined");
		if (flags & FLAG_INTERACTIVE)
		    c_write_str(ssh, "TIS authentication refused.\r\n");
		s->tis_auth_refused = 1;
		continue;
	    } else {
		char *challenge;
		int challengelen;
		char *instr_suf, *prompt;

		ssh_pkt_getstring(pktin, &challenge, &challengelen);
		if (!challenge) {
		    bombout(("TIS challenge packet was badly formed"));
		    crStop(0);
		}
		logevent("Received TIS challenge");
		s->cur_prompt->to_server = TRUE;
		s->cur_prompt->name = dupstr("SSH TIS authentication");
		/* Prompt heuristic comes from OpenSSH */
		if (memchr(challenge, '\n', challengelen)) {
		    instr_suf = dupstr("");
		    prompt = dupprintf("%.*s", challengelen, challenge);
		} else {
		    instr_suf = dupprintf("%.*s", challengelen, challenge);
		    prompt = dupstr("Response: ");
		}
		s->cur_prompt->instruction =
		    dupprintf("Using TIS authentication.%s%s",
			      (*instr_suf) ? "\n" : "",
			      instr_suf);
		s->cur_prompt->instr_reqd = TRUE;
		add_prompt(s->cur_prompt, prompt, FALSE);
		sfree(instr_suf);
	    }
	}
	if (conf_get_int(ssh->conf, CONF_try_tis_auth) &&
	    (s->supported_auths_mask & (1 << SSH1_AUTH_CCARD)) &&
	    !s->ccard_auth_refused) {
	    s->pwpkt_type = SSH1_CMSG_AUTH_CCARD_RESPONSE;
	    logevent("Requested CryptoCard authentication");
	    send_packet(ssh, SSH1_CMSG_AUTH_CCARD, PKT_END);
	    crWaitUntil(pktin);
	    if (pktin->type != SSH1_SMSG_AUTH_CCARD_CHALLENGE) {
		logevent("CryptoCard authentication declined");
		c_write_str(ssh, "CryptoCard authentication refused.\r\n");
		s->ccard_auth_refused = 1;
		continue;
	    } else {
		char *challenge;
		int challengelen;
		char *instr_suf, *prompt;

		ssh_pkt_getstring(pktin, &challenge, &challengelen);
		if (!challenge) {
		    bombout(("CryptoCard challenge packet was badly formed"));
		    crStop(0);
		}
		logevent("Received CryptoCard challenge");
		s->cur_prompt->to_server = TRUE;
		s->cur_prompt->name = dupstr("SSH CryptoCard authentication");
		s->cur_prompt->name_reqd = FALSE;
		/* Prompt heuristic comes from OpenSSH */
		if (memchr(challenge, '\n', challengelen)) {
		    instr_suf = dupstr("");
		    prompt = dupprintf("%.*s", challengelen, challenge);
		} else {
		    instr_suf = dupprintf("%.*s", challengelen, challenge);
		    prompt = dupstr("Response: ");
		}
		s->cur_prompt->instruction =
		    dupprintf("Using CryptoCard authentication.%s%s",
			      (*instr_suf) ? "\n" : "",
			      instr_suf);
		s->cur_prompt->instr_reqd = TRUE;
		add_prompt(s->cur_prompt, prompt, FALSE);
		sfree(instr_suf);
	    }
	}
	if (s->pwpkt_type == SSH1_CMSG_AUTH_PASSWORD) {
	    if ((s->supported_auths_mask & (1 << SSH1_AUTH_PASSWORD)) == 0) {
		bombout(("No supported authentication methods available"));
		crStop(0);
	    }
	    s->cur_prompt->to_server = TRUE;
	    s->cur_prompt->name = dupstr("SSH password");
	    add_prompt(s->cur_prompt, dupprintf("%s@%s's password: ",
						ssh->username, ssh->savedhost),
		       FALSE);
	}

	/*
	 * Show password prompt, having first obtained it via a TIS
	 * or CryptoCard exchange if we're doing TIS or CryptoCard
	 * authentication.
	 */
	{
	    int ret; /* need not be kept over crReturn */
	    ret = get_userpass_input(s->cur_prompt, NULL, 0);
	    while (ret < 0) {
		ssh->send_ok = 1;
		crWaitUntil(!pktin);
		ret = get_userpass_input(s->cur_prompt, in, inlen);
		ssh->send_ok = 0;
	    }
	    if (!ret) {
		/*
		 * Failed to get a password (for example
		 * because one was supplied on the command line
		 * which has already failed to work). Terminate.
		 */
		free_prompts(s->cur_prompt);
		ssh_disconnect(ssh, NULL, "Unable to authenticate", 0, TRUE);
		crStop(0);
	    }
	}

	if (s->pwpkt_type == SSH1_CMSG_AUTH_PASSWORD) {
	    /*
	     * Defence against traffic analysis: we send a
	     * whole bunch of packets containing strings of
	     * different lengths. One of these strings is the
	     * password, in a SSH1_CMSG_AUTH_PASSWORD packet.
	     * The others are all random data in
	     * SSH1_MSG_IGNORE packets. This way a passive
	     * listener can't tell which is the password, and
	     * hence can't deduce the password length.
	     * 
	     * Anybody with a password length greater than 16
	     * bytes is going to have enough entropy in their
	     * password that a listener won't find it _that_
	     * much help to know how long it is. So what we'll
	     * do is:
	     * 
	     *  - if password length < 16, we send 15 packets
	     *    containing string lengths 1 through 15
	     * 
	     *  - otherwise, we let N be the nearest multiple
	     *    of 8 below the password length, and send 8
	     *    packets containing string lengths N through
	     *    N+7. This won't obscure the order of
	     *    magnitude of the password length, but it will
	     *    introduce a bit of extra uncertainty.
	     * 
	     * A few servers can't deal with SSH1_MSG_IGNORE, at
	     * least in this context. For these servers, we need
	     * an alternative defence. We make use of the fact
	     * that the password is interpreted as a C string:
	     * so we can append a NUL, then some random data.
	     * 
	     * A few servers can deal with neither SSH1_MSG_IGNORE
	     * here _nor_ a padded password string.
	     * For these servers we are left with no defences
	     * against password length sniffing.
	     */
	    if (!(ssh->remote_bugs & BUG_CHOKES_ON_SSH1_IGNORE) &&
	        !(ssh->remote_bugs & BUG_NEEDS_SSH1_PLAIN_PASSWORD)) {
		/*
		 * The server can deal with SSH1_MSG_IGNORE, so
		 * we can use the primary defence.
		 */
		int bottom, top, pwlen, i;
		char *randomstr;

		pwlen = strlen(s->cur_prompt->prompts[0]->result);
		if (pwlen < 16) {
		    bottom = 0;    /* zero length passwords are OK! :-) */
		    top = 15;
		} else {
		    bottom = pwlen & ~7;
		    top = bottom + 7;
		}

		assert(pwlen >= bottom && pwlen <= top);

		randomstr = snewn(top + 1, char);

		for (i = bottom; i <= top; i++) {
		    if (i == pwlen) {
			defer_packet(ssh, s->pwpkt_type,
                                     PKT_STR,s->cur_prompt->prompts[0]->result,
				     PKT_END);
		    } else {
			for (j = 0; j < i; j++) {
			    do {
				randomstr[j] = random_byte();
			    } while (randomstr[j] == '\0');
			}
			randomstr[i] = '\0';
			defer_packet(ssh, SSH1_MSG_IGNORE,
				     PKT_STR, randomstr, PKT_END);
		    }
		}
		logevent("Sending password with camouflage packets");
		ssh_pkt_defersend(ssh);
		sfree(randomstr);
	    } 
	    else if (!(ssh->remote_bugs & BUG_NEEDS_SSH1_PLAIN_PASSWORD)) {
		/*
		 * The server can't deal with SSH1_MSG_IGNORE
		 * but can deal with padded passwords, so we
		 * can use the secondary defence.
		 */
		char string[64];
		char *ss;
		int len;

		len = strlen(s->cur_prompt->prompts[0]->result);
		if (len < sizeof(string)) {
		    ss = string;
		    strcpy(string, s->cur_prompt->prompts[0]->result);
		    len++;	       /* cover the zero byte */
		    while (len < sizeof(string)) {
			string[len++] = (char) random_byte();
		    }
		} else {
		    ss = s->cur_prompt->prompts[0]->result;
		}
		logevent("Sending length-padded password");
		send_packet(ssh, s->pwpkt_type,
			    PKT_INT, len, PKT_DATA, ss, len,
			    PKT_END);
	    } else {
		/*
		 * The server is believed unable to cope with
		 * any of our password camouflage methods.
		 */
		int len;
		len = strlen(s->cur_prompt->prompts[0]->result);
		logevent("Sending unpadded password");
		send_packet(ssh, s->pwpkt_type,
                            PKT_INT, len,
			    PKT_DATA, s->cur_prompt->prompts[0]->result, len,
			    PKT_END);
	    }
	} else {
	    send_packet(ssh, s->pwpkt_type,
			PKT_STR, s->cur_prompt->prompts[0]->result,
			PKT_END);
	}
	logevent("Sent password");
	free_prompts(s->cur_prompt);
	crWaitUntil(pktin);
	if (pktin->type == SSH1_SMSG_FAILURE) {
	    if (flags & FLAG_VERBOSE)
		c_write_str(ssh, "Access denied\r\n");
	    logevent("Authentication refused");
	} else if (pktin->type != SSH1_SMSG_SUCCESS) {
	    bombout(("Strange packet received, type %d", pktin->type));
	    crStop(0);
	}
    }

    /* Clear up */
    if (s->publickey_blob) {
	sfree(s->publickey_blob);
	sfree(s->publickey_comment);
    }

    logevent("Authentication successful");

    crFinish(1);
}

static void ssh_channel_try_eof(struct ssh_channel *c)
{
    Ssh ssh = c->ssh;
    assert(c->pending_eof);          /* precondition for calling us */
    if (c->halfopen)
        return;                 /* can't close: not even opened yet */
    if (ssh->version == 2 && bufchain_size(&c->v.v2.outbuffer) > 0)
        return;              /* can't send EOF: pending outgoing data */

    c->pending_eof = FALSE;            /* we're about to send it */
    if (ssh->version == 1) {
        send_packet(ssh, SSH1_MSG_CHANNEL_CLOSE, PKT_INT, c->remoteid,
                    PKT_END);
        c->closes |= CLOSES_SENT_EOF;
    } else {
        struct Packet *pktout;
        pktout = ssh2_pkt_init(SSH2_MSG_CHANNEL_EOF);
        ssh2_pkt_adduint32(pktout, c->remoteid);
        ssh2_pkt_send(ssh, pktout);
        c->closes |= CLOSES_SENT_EOF;
	ssh2_channel_check_close(c);
    }
}

Conf *sshfwd_get_conf(struct ssh_channel *c)
{
    Ssh ssh = c->ssh;
    return ssh->conf;
}

void sshfwd_write_eof(struct ssh_channel *c)
{
    Ssh ssh = c->ssh;

    if (ssh->state == SSH_STATE_CLOSED)
	return;

    if (c->closes & CLOSES_SENT_EOF)
        return;

    c->pending_eof = TRUE;
    ssh_channel_try_eof(c);
}

void sshfwd_unclean_close(struct ssh_channel *c, const char *err)
{
    Ssh ssh = c->ssh;
    char *reason;

    if (ssh->state == SSH_STATE_CLOSED)
	return;

    reason = dupprintf("due to local error: %s", err);
    ssh_channel_close_local(c, reason);
    sfree(reason);
    c->pending_eof = FALSE;   /* this will confuse a zombie channel */

    ssh2_channel_check_close(c);
}

int sshfwd_write(struct ssh_channel *c, char *buf, int len)
{
    Ssh ssh = c->ssh;

    if (ssh->state == SSH_STATE_CLOSED)
	return 0;

    return ssh_send_channel_data(c, buf, len);
}

void sshfwd_unthrottle(struct ssh_channel *c, int bufsize)
{
    Ssh ssh = c->ssh;

    if (ssh->state == SSH_STATE_CLOSED)
	return;

    ssh_channel_unthrottle(c, bufsize);
}

static void ssh_queueing_handler(Ssh ssh, struct Packet *pktin)
{
    struct queued_handler *qh = ssh->qhead;

    assert(qh != NULL);

    assert(pktin->type == qh->msg1 || pktin->type == qh->msg2);

    if (qh->msg1 > 0) {
	assert(ssh->packet_dispatch[qh->msg1] == ssh_queueing_handler);
	ssh->packet_dispatch[qh->msg1] = ssh->q_saved_handler1;
    }
    if (qh->msg2 > 0) {
	assert(ssh->packet_dispatch[qh->msg2] == ssh_queueing_handler);
	ssh->packet_dispatch[qh->msg2] = ssh->q_saved_handler2;
    }

    if (qh->next) {
	ssh->qhead = qh->next;

	if (ssh->qhead->msg1 > 0) {
	    ssh->q_saved_handler1 = ssh->packet_dispatch[ssh->qhead->msg1];
	    ssh->packet_dispatch[ssh->qhead->msg1] = ssh_queueing_handler;
	}
	if (ssh->qhead->msg2 > 0) {
	    ssh->q_saved_handler2 = ssh->packet_dispatch[ssh->qhead->msg2];
	    ssh->packet_dispatch[ssh->qhead->msg2] = ssh_queueing_handler;
	}
    } else {
	ssh->qhead = ssh->qtail = NULL;
    }

    qh->handler(ssh, pktin, qh->ctx);

    sfree(qh);
}

static void ssh_queue_handler(Ssh ssh, int msg1, int msg2,
			      chandler_fn_t handler, void *ctx)
{
    struct queued_handler *qh;

    qh = snew(struct queued_handler);
    qh->msg1 = msg1;
    qh->msg2 = msg2;
    qh->handler = handler;
    qh->ctx = ctx;
    qh->next = NULL;

    if (ssh->qtail == NULL) {
	ssh->qhead = qh;

	if (qh->msg1 > 0) {
	    ssh->q_saved_handler1 = ssh->packet_dispatch[ssh->qhead->msg1];
	    ssh->packet_dispatch[qh->msg1] = ssh_queueing_handler;
	}
	if (qh->msg2 > 0) {
	    ssh->q_saved_handler2 = ssh->packet_dispatch[ssh->qhead->msg2];
	    ssh->packet_dispatch[qh->msg2] = ssh_queueing_handler;
	}
    } else {
	ssh->qtail->next = qh;
    }
    ssh->qtail = qh;
}

static void ssh_rportfwd_succfail(Ssh ssh, struct Packet *pktin, void *ctx)
{
    struct ssh_rportfwd *rpf, *pf = (struct ssh_rportfwd *)ctx;

    if (pktin->type == (ssh->version == 1 ? SSH1_SMSG_SUCCESS :
			SSH2_MSG_REQUEST_SUCCESS)) {
	logeventf(ssh, "Remote port forwarding from %s enabled",
		  pf->sportdesc);
    } else {
	logeventf(ssh, "Remote port forwarding from %s refused",
		  pf->sportdesc);

	rpf = del234(ssh->rportfwds, pf);
	assert(rpf == pf);
	pf->pfrec->remote = NULL;
	free_rportfwd(pf);
    }
}

int ssh_alloc_sharing_rportfwd(Ssh ssh, const char *shost, int sport,
                               void *share_ctx)
{
    struct ssh_rportfwd *pf = snew(struct ssh_rportfwd);
    pf->dhost = NULL;
    pf->dport = 0;
    pf->share_ctx = share_ctx;
    pf->shost = dupstr(shost);
    pf->sport = sport;
    pf->sportdesc = NULL;
    if (!ssh->rportfwds) {
        assert(ssh->version == 2);
        ssh->rportfwds = newtree234(ssh_rportcmp_ssh2);
    }
    if (add234(ssh->rportfwds, pf) != pf) {
        sfree(pf->shost);
        sfree(pf);
        return FALSE;
    }
    return TRUE;
}

static void ssh_sharing_global_request_response(Ssh ssh, struct Packet *pktin,
                                                void *ctx)
{
    share_got_pkt_from_server(ctx, pktin->type,
                              pktin->body, pktin->length);
}

void ssh_sharing_queue_global_request(Ssh ssh, void *share_ctx)
{
    ssh_queue_handler(ssh, SSH2_MSG_REQUEST_SUCCESS, SSH2_MSG_REQUEST_FAILURE,
                      ssh_sharing_global_request_response, share_ctx);
}

static void ssh_setup_portfwd(Ssh ssh, Conf *conf)
{
    struct ssh_portfwd *epf;
    int i;
    char *key, *val;

    if (!ssh->portfwds) {
	ssh->portfwds = newtree234(ssh_portcmp);
    } else {
	/*
	 * Go through the existing port forwardings and tag them
	 * with status==DESTROY. Any that we want to keep will be
	 * re-enabled (status==KEEP) as we go through the
	 * configuration and find out which bits are the same as
	 * they were before.
	 */
	struct ssh_portfwd *epf;
	int i;
	for (i = 0; (epf = index234(ssh->portfwds, i)) != NULL; i++)
	    epf->status = DESTROY;
    }

    for (val = conf_get_str_strs(conf, CONF_portfwd, NULL, &key);
	 val != NULL;
	 val = conf_get_str_strs(conf, CONF_portfwd, key, &key)) {
	char *kp, *kp2, *vp, *vp2;
	char address_family, type;
	int sport,dport,sserv,dserv;
	char *sports, *dports, *saddr, *host;

	kp = key;

	address_family = 'A';
	type = 'L';
	if (*kp == 'A' || *kp == '4' || *kp == '6')
	    address_family = *kp++;
	if (*kp == 'L' || *kp == 'R')
	    type = *kp++;

	if ((kp2 = host_strchr(kp, ':')) != NULL) {
	    /*
	     * There's a colon in the middle of the source port
	     * string, which means that the part before it is
	     * actually a source address.
	     */
	    char *saddr_tmp = dupprintf("%.*s", (int)(kp2 - kp), kp);
            saddr = host_strduptrim(saddr_tmp);
            sfree(saddr_tmp);
	    sports = kp2+1;
	} else {
	    saddr = NULL;
	    sports = kp;
	}
	sport = atoi(sports);
	sserv = 0;
	if (sport == 0) {
	    sserv = 1;
	    sport = net_service_lookup(sports);
	    if (!sport) {
		logeventf(ssh, "Service lookup failed for source"
			  " port \"%s\"", sports);
	    }
	}

	if (type == 'L' && !strcmp(val, "D")) {
            /* dynamic forwarding */
	    host = NULL;
	    dports = NULL;
	    dport = -1;
	    dserv = 0;
            type = 'D';
        } else {
            /* ordinary forwarding */
	    vp = val;
	    vp2 = vp + host_strcspn(vp, ":");
	    host = dupprintf("%.*s", (int)(vp2 - vp), vp);
	    if (*vp2)
		vp2++;
	    dports = vp2;
	    dport = atoi(dports);
	    dserv = 0;
	    if (dport == 0) {
		dserv = 1;
		dport = net_service_lookup(dports);
		if (!dport) {
		    logeventf(ssh, "Service lookup failed for destination"
			      " port \"%s\"", dports);
		}
	    }
	}

	if (sport && dport) {
	    /* Set up a description of the source port. */
	    struct ssh_portfwd *pfrec, *epfrec;

	    pfrec = snew(struct ssh_portfwd);
	    pfrec->type = type;
	    pfrec->saddr = saddr;
	    pfrec->sserv = sserv ? dupstr(sports) : NULL;
	    pfrec->sport = sport;
	    pfrec->daddr = host;
	    pfrec->dserv = dserv ? dupstr(dports) : NULL;
	    pfrec->dport = dport;
	    pfrec->local = NULL;
	    pfrec->remote = NULL;
	    pfrec->addressfamily = (address_family == '4' ? ADDRTYPE_IPV4 :
				    address_family == '6' ? ADDRTYPE_IPV6 :
				    ADDRTYPE_UNSPEC);

	    epfrec = add234(ssh->portfwds, pfrec);
	    if (epfrec != pfrec) {
		if (epfrec->status == DESTROY) {
		    /*
		     * We already have a port forwarding up and running
		     * with precisely these parameters. Hence, no need
		     * to do anything; simply re-tag the existing one
		     * as KEEP.
		     */
		    epfrec->status = KEEP;
		}
		/*
		 * Anything else indicates that there was a duplicate
		 * in our input, which we'll silently ignore.
		 */
		free_portfwd(pfrec);
	    } else {
		pfrec->status = CREATE;
	    }
	} else {
	    sfree(saddr);
	    sfree(host);
	}
    }

    /*
     * Now go through and destroy any port forwardings which were
     * not re-enabled.
     */
    for (i = 0; (epf = index234(ssh->portfwds, i)) != NULL; i++)
	if (epf->status == DESTROY) {
	    char *message;

	    message = dupprintf("%s port forwarding from %s%s%d",
				epf->type == 'L' ? "local" :
				epf->type == 'R' ? "remote" : "dynamic",
				epf->saddr ? epf->saddr : "",
				epf->saddr ? ":" : "",
				epf->sport);

	    if (epf->type != 'D') {
		char *msg2 = dupprintf("%s to %s:%d", message,
				       epf->daddr, epf->dport);
		sfree(message);
		message = msg2;
	    }

	    logeventf(ssh, "Cancelling %s", message);
	    sfree(message);

	    /* epf->remote or epf->local may be NULL if setting up a
	     * forwarding failed. */
	    if (epf->remote) {
		struct ssh_rportfwd *rpf = epf->remote;
		struct Packet *pktout;

		/*
		 * Cancel the port forwarding at the server
		 * end.
		 */
		if (ssh->version == 1) {
		    /*
		     * We cannot cancel listening ports on the
		     * server side in SSH-1! There's no message
		     * to support it. Instead, we simply remove
		     * the rportfwd record from the local end
		     * so that any connections the server tries
		     * to make on it are rejected.
		     */
		} else {
		    pktout = ssh2_pkt_init(SSH2_MSG_GLOBAL_REQUEST);
		    ssh2_pkt_addstring(pktout, "cancel-tcpip-forward");
		    ssh2_pkt_addbool(pktout, 0);/* _don't_ want reply */
		    if (epf->saddr) {
			ssh2_pkt_addstring(pktout, epf->saddr);
		    } else if (conf_get_int(conf, CONF_rport_acceptall)) {
			/* XXX: rport_acceptall may not represent
			 * what was used to open the original connection,
			 * since it's reconfigurable. */
			ssh2_pkt_addstring(pktout, "");
		    } else {
			ssh2_pkt_addstring(pktout, "localhost");
		    }
		    ssh2_pkt_adduint32(pktout, epf->sport);
		    ssh2_pkt_send(ssh, pktout);
		}

		del234(ssh->rportfwds, rpf);
		free_rportfwd(rpf);
	    } else if (epf->local) {
		pfl_terminate(epf->local);
	    }

	    delpos234(ssh->portfwds, i);
	    free_portfwd(epf);
	    i--;		       /* so we don't skip one in the list */
	}

    /*
     * And finally, set up any new port forwardings (status==CREATE).
     */
    for (i = 0; (epf = index234(ssh->portfwds, i)) != NULL; i++)
	if (epf->status == CREATE) {
	    char *sportdesc, *dportdesc;
	    sportdesc = dupprintf("%s%s%s%s%d%s",
				  epf->saddr ? epf->saddr : "",
				  epf->saddr ? ":" : "",
				  epf->sserv ? epf->sserv : "",
				  epf->sserv ? "(" : "",
				  epf->sport,
				  epf->sserv ? ")" : "");
	    if (epf->type == 'D') {
		dportdesc = NULL;
	    } else {
		dportdesc = dupprintf("%s:%s%s%d%s",
				      epf->daddr,
				      epf->dserv ? epf->dserv : "",
				      epf->dserv ? "(" : "",
				      epf->dport,
				      epf->dserv ? ")" : "");
	    }

	    if (epf->type == 'L') {
                char *err = pfl_listen(epf->daddr, epf->dport,
                                       epf->saddr, epf->sport,
                                       ssh, conf, &epf->local,
                                       epf->addressfamily);

		logeventf(ssh, "Local %sport %s forwarding to %s%s%s",
			  epf->addressfamily == ADDRTYPE_IPV4 ? "IPv4 " :
			  epf->addressfamily == ADDRTYPE_IPV6 ? "IPv6 " : "",
			  sportdesc, dportdesc,
			  err ? " failed: " : "", err ? err : "");
                if (err)
                    sfree(err);
	    } else if (epf->type == 'D') {
		char *err = pfl_listen(NULL, -1, epf->saddr, epf->sport,
                                       ssh, conf, &epf->local,
                                       epf->addressfamily);

		logeventf(ssh, "Local %sport %s SOCKS dynamic forwarding%s%s",
			  epf->addressfamily == ADDRTYPE_IPV4 ? "IPv4 " :
			  epf->addressfamily == ADDRTYPE_IPV6 ? "IPv6 " : "",
			  sportdesc,
			  err ? " failed: " : "", err ? err : "");

                if (err)
                    sfree(err);
	    } else {
		struct ssh_rportfwd *pf;

		/*
		 * Ensure the remote port forwardings tree exists.
		 */
		if (!ssh->rportfwds) {
		    if (ssh->version == 1)
			ssh->rportfwds = newtree234(ssh_rportcmp_ssh1);
		    else
			ssh->rportfwds = newtree234(ssh_rportcmp_ssh2);
		}

		pf = snew(struct ssh_rportfwd);
                pf->share_ctx = NULL;
                pf->dhost = dupstr(epf->daddr);
		pf->dport = epf->dport;
                if (epf->saddr) {
                    pf->shost = dupstr(epf->saddr);
                } else if (conf_get_int(conf, CONF_rport_acceptall)) {
                    pf->shost = dupstr("");
                } else {
                    pf->shost = dupstr("localhost");
                }
		pf->sport = epf->sport;
		if (add234(ssh->rportfwds, pf) != pf) {
		    logeventf(ssh, "Duplicate remote port forwarding to %s:%d",
			      epf->daddr, epf->dport);
		    sfree(pf);
		} else {
		    logeventf(ssh, "Requesting remote port %s"
			      " forward to %s", sportdesc, dportdesc);

		    pf->sportdesc = sportdesc;
		    sportdesc = NULL;
		    epf->remote = pf;
		    pf->pfrec = epf;

		    if (ssh->version == 1) {
			send_packet(ssh, SSH1_CMSG_PORT_FORWARD_REQUEST,
				    PKT_INT, epf->sport,
				    PKT_STR, epf->daddr,
				    PKT_INT, epf->dport,
				    PKT_END);
			ssh_queue_handler(ssh, SSH1_SMSG_SUCCESS,
					  SSH1_SMSG_FAILURE,
					  ssh_rportfwd_succfail, pf);
		    } else {
			struct Packet *pktout;
			pktout = ssh2_pkt_init(SSH2_MSG_GLOBAL_REQUEST);
			ssh2_pkt_addstring(pktout, "tcpip-forward");
			ssh2_pkt_addbool(pktout, 1);/* want reply */
			ssh2_pkt_addstring(pktout, pf->shost);
			ssh2_pkt_adduint32(pktout, pf->sport);
			ssh2_pkt_send(ssh, pktout);

			ssh_queue_handler(ssh, SSH2_MSG_REQUEST_SUCCESS,
					  SSH2_MSG_REQUEST_FAILURE,
					  ssh_rportfwd_succfail, pf);
		    }
		}
	    }
	    sfree(sportdesc);
	    sfree(dportdesc);
	}
}

static void ssh1_smsg_stdout_stderr_data(Ssh ssh, struct Packet *pktin)
{
    char *string;
    int stringlen, bufsize;

    ssh_pkt_getstring(pktin, &string, &stringlen);
    if (string == NULL) {
	bombout(("Incoming terminal data packet was badly formed"));
	return;
    }

    bufsize = from_backend(ssh->frontend, pktin->type == SSH1_SMSG_STDERR_DATA,
			   string, stringlen);
    if (!ssh->v1_stdout_throttling && bufsize > SSH1_BUFFER_LIMIT) {
	ssh->v1_stdout_throttling = 1;
	ssh_throttle_conn(ssh, +1);
    }
}

static void ssh1_smsg_x11_open(Ssh ssh, struct Packet *pktin)
{
    /* Remote side is trying to open a channel to talk to our
     * X-Server. Give them back a local channel number. */
    struct ssh_channel *c;
    int remoteid = ssh_pkt_getuint32(pktin);

    logevent("Received X11 connect request");
    /* Refuse if X11 forwarding is disabled. */
    if (!ssh->X11_fwd_enabled) {
	send_packet(ssh, SSH1_MSG_CHANNEL_OPEN_FAILURE,
		    PKT_INT, remoteid, PKT_END);
	logevent("Rejected X11 connect request");
    } else {
	c = snew(struct ssh_channel);
	c->ssh = ssh;

	ssh_channel_init(c);
	c->u.x11.xconn = x11_init(ssh->x11authtree, c, NULL, -1);
        c->remoteid = remoteid;
        c->halfopen = FALSE;
        c->type = CHAN_X11;	/* identify channel type */
        send_packet(ssh, SSH1_MSG_CHANNEL_OPEN_CONFIRMATION,
                    PKT_INT, c->remoteid, PKT_INT,
                    c->localid, PKT_END);
        logevent("Opened X11 forward channel");
    }
}

static void ssh1_smsg_agent_open(Ssh ssh, struct Packet *pktin)
{
    /* Remote side is trying to open a channel to talk to our
     * agent. Give them back a local channel number. */
    struct ssh_channel *c;
    int remoteid = ssh_pkt_getuint32(pktin);

    /* Refuse if agent forwarding is disabled. */
    if (!ssh->agentfwd_enabled) {
	send_packet(ssh, SSH1_MSG_CHANNEL_OPEN_FAILURE,
		    PKT_INT, remoteid, PKT_END);
    } else {
	c = snew(struct ssh_channel);
	c->ssh = ssh;
	ssh_channel_init(c);
	c->remoteid = remoteid;
	c->halfopen = FALSE;
	c->type = CHAN_AGENT;	/* identify channel type */
	c->u.a.lensofar = 0;
	c->u.a.message = NULL;
	c->u.a.pending = NULL;
	c->u.a.outstanding_requests = 0;
	send_packet(ssh, SSH1_MSG_CHANNEL_OPEN_CONFIRMATION,
		    PKT_INT, c->remoteid, PKT_INT, c->localid,
		    PKT_END);
    }
}

static void ssh1_msg_port_open(Ssh ssh, struct Packet *pktin)
{
    /* Remote side is trying to open a channel to talk to a
     * forwarded port. Give them back a local channel number. */
    struct ssh_rportfwd pf, *pfp;
    int remoteid;
    int hostsize, port;
    char *host;
    char *err;

    remoteid = ssh_pkt_getuint32(pktin);
    ssh_pkt_getstring(pktin, &host, &hostsize);
    port = ssh_pkt_getuint32(pktin);

    pf.dhost = dupprintf("%.*s", hostsize, NULLTOEMPTY(host));
    pf.dport = port;
    pfp = find234(ssh->rportfwds, &pf, NULL);

    if (pfp == NULL) {
	logeventf(ssh, "Rejected remote port open request for %s:%d",
		  pf.dhost, port);
	send_packet(ssh, SSH1_MSG_CHANNEL_OPEN_FAILURE,
		    PKT_INT, remoteid, PKT_END);
    } else {
        struct ssh_channel *c = snew(struct ssh_channel);
        c->ssh = ssh;

	logeventf(ssh, "Received remote port open request for %s:%d",
		  pf.dhost, port);
	err = pfd_connect(&c->u.pfd.pf, pf.dhost, port,
                          c, ssh->conf, pfp->pfrec->addressfamily);
	if (err != NULL) {
	    logeventf(ssh, "Port open failed: %s", err);
            sfree(err);
	    sfree(c);
	    send_packet(ssh, SSH1_MSG_CHANNEL_OPEN_FAILURE,
			PKT_INT, remoteid, PKT_END);
	} else {
	    ssh_channel_init(c);
	    c->remoteid = remoteid;
	    c->halfopen = FALSE;
	    c->type = CHAN_SOCKDATA;	/* identify channel type */
	    send_packet(ssh, SSH1_MSG_CHANNEL_OPEN_CONFIRMATION,
			PKT_INT, c->remoteid, PKT_INT,
			c->localid, PKT_END);
	    logevent("Forwarded port opened successfully");
	}
    }

    sfree(pf.dhost);
}

static void ssh1_msg_channel_open_confirmation(Ssh ssh, struct Packet *pktin)
{
    struct ssh_channel *c;

    c = ssh_channel_msg(ssh, pktin);
    if (c && c->type == CHAN_SOCKDATA) {
	c->remoteid = ssh_pkt_getuint32(pktin);
	c->halfopen = FALSE;
	c->throttling_conn = 0;
	pfd_confirm(c->u.pfd.pf);
    }

    if (c && c->pending_eof) {
	/*
	 * We have a pending close on this channel,
	 * which we decided on before the server acked
	 * the channel open. So now we know the
	 * remoteid, we can close it again.
	 */
        ssh_channel_try_eof(c);
    }
}

        c->remoteid = remoteid;
        c->halfopen = FALSE;
        c->type = CHAN_AGENT;   /* identify channel type */
        c->u.a.pending = NULL;
        bufchain_init(&c->u.a.inbuffer);
        send_packet(ssh, SSH1_MSG_CHANNEL_OPEN_CONFIRMATION,
                    PKT_INT, c->remoteid, PKT_INT, c->localid,
                    PKT_END);
	del234(ssh->channels, c);
	sfree(c);
    }
}
