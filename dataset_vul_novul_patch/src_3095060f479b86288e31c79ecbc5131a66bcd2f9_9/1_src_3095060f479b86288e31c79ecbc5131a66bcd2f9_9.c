ssh_kex2(char *host, struct sockaddr *hostaddr, u_short port)
{
	char *myproposal[PROPOSAL_MAX] = { KEX_CLIENT };
	char *s;
	struct kex *kex;
	int r;

	xxx_host = host;
	xxx_hostaddr = hostaddr;

	if ((s = kex_names_cat(options.kex_algorithms, "ext-info-c")) == NULL)
		fatal("%s: kex_names_cat", __func__);
	myproposal[PROPOSAL_KEX_ALGS] = compat_kex_proposal(s);
	myproposal[PROPOSAL_ENC_ALGS_CTOS] =
	    compat_cipher_proposal(options.ciphers);
	myproposal[PROPOSAL_ENC_ALGS_STOC] =
 	    compat_cipher_proposal(options.ciphers);
 	myproposal[PROPOSAL_COMP_ALGS_CTOS] =
 	    myproposal[PROPOSAL_COMP_ALGS_STOC] = options.compression ?
	    "zlib@openssh.com,zlib,none" : "none,zlib@openssh.com,zlib";
 	myproposal[PROPOSAL_MAC_ALGS_CTOS] =
 	    myproposal[PROPOSAL_MAC_ALGS_STOC] = options.macs;
 	if (options.hostkeyalgorithms != NULL) {
		if (kex_assemble_names(KEX_DEFAULT_PK_ALG,
		    &options.hostkeyalgorithms) != 0)
			fatal("%s: kex_assemble_namelist", __func__);
		myproposal[PROPOSAL_SERVER_HOST_KEY_ALGS] =
		    compat_pkalg_proposal(options.hostkeyalgorithms);
	} else {
		/* Enforce default */
		options.hostkeyalgorithms = xstrdup(KEX_DEFAULT_PK_ALG);
		/* Prefer algorithms that we already have keys for */
		myproposal[PROPOSAL_SERVER_HOST_KEY_ALGS] =
		    compat_pkalg_proposal(
		    order_hostkeyalgs(host, hostaddr, port));
	}

	if (options.rekey_limit || options.rekey_interval)
		packet_set_rekey_limits((u_int32_t)options.rekey_limit,
		    (time_t)options.rekey_interval);

	/* start key exchange */
	if ((r = kex_setup(active_state, myproposal)) != 0)
		fatal("kex_setup: %s", ssh_err(r));
	kex = active_state->kex;
#ifdef WITH_OPENSSL
	kex->kex[KEX_DH_GRP1_SHA1] = kexdh_client;
	kex->kex[KEX_DH_GRP14_SHA1] = kexdh_client;
	kex->kex[KEX_DH_GRP14_SHA256] = kexdh_client;
	kex->kex[KEX_DH_GRP16_SHA512] = kexdh_client;
	kex->kex[KEX_DH_GRP18_SHA512] = kexdh_client;
	kex->kex[KEX_DH_GEX_SHA1] = kexgex_client;
	kex->kex[KEX_DH_GEX_SHA256] = kexgex_client;
	kex->kex[KEX_ECDH_SHA2] = kexecdh_client;
#endif
	kex->kex[KEX_C25519_SHA256] = kexc25519_client;
	kex->client_version_string=client_version_string;
	kex->server_version_string=server_version_string;
	kex->verify_host_key=&verify_host_key_callback;

	dispatch_run(DISPATCH_BLOCK, &kex->done, active_state);

	/* remove ext-info from the KEX proposals for rekeying */
	myproposal[PROPOSAL_KEX_ALGS] =
	    compat_kex_proposal(options.kex_algorithms);
	if ((r = kex_prop2buf(kex->my, myproposal)) != 0)
		fatal("kex_prop2buf: %s", ssh_err(r));

	session_id2 = kex->session_id;
	session_id2_len = kex->session_id_len;

#ifdef DEBUG_KEXDH
	/* send 1st encrypted/maced/compressed message */
	packet_start(SSH2_MSG_IGNORE);
	packet_put_cstring("markus");
	packet_send();
	packet_write_wait();
#endif
}
