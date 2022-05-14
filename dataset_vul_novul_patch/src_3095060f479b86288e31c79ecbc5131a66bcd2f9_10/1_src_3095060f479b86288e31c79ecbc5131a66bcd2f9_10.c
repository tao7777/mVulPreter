do_ssh2_kex(void)
{
	char *myproposal[PROPOSAL_MAX] = { KEX_SERVER };
	struct kex *kex;
	int r;

	myproposal[PROPOSAL_KEX_ALGS] = compat_kex_proposal(
	    options.kex_algorithms);
	myproposal[PROPOSAL_ENC_ALGS_CTOS] = compat_cipher_proposal(
	    options.ciphers);
	myproposal[PROPOSAL_ENC_ALGS_STOC] = compat_cipher_proposal(
	    options.ciphers);
	myproposal[PROPOSAL_MAC_ALGS_CTOS] =
	    myproposal[PROPOSAL_MAC_ALGS_STOC] = options.macs;

 	if (options.compression == COMP_NONE) {
 		myproposal[PROPOSAL_COMP_ALGS_CTOS] =
 		    myproposal[PROPOSAL_COMP_ALGS_STOC] = "none";
	} else if (options.compression == COMP_DELAYED) {
		myproposal[PROPOSAL_COMP_ALGS_CTOS] =
		    myproposal[PROPOSAL_COMP_ALGS_STOC] =
		    "none,zlib@openssh.com";
 	}
 
 	if (options.rekey_limit || options.rekey_interval)
		packet_set_rekey_limits(options.rekey_limit,
		    (time_t)options.rekey_interval);

	myproposal[PROPOSAL_SERVER_HOST_KEY_ALGS] = compat_pkalg_proposal(
	    list_hostkey_types());

	/* start key exchange */
	if ((r = kex_setup(active_state, myproposal)) != 0)
		fatal("kex_setup: %s", ssh_err(r));
	kex = active_state->kex;
#ifdef WITH_OPENSSL
	kex->kex[KEX_DH_GRP1_SHA1] = kexdh_server;
	kex->kex[KEX_DH_GRP14_SHA1] = kexdh_server;
	kex->kex[KEX_DH_GRP14_SHA256] = kexdh_server;
	kex->kex[KEX_DH_GRP16_SHA512] = kexdh_server;
	kex->kex[KEX_DH_GRP18_SHA512] = kexdh_server;
	kex->kex[KEX_DH_GEX_SHA1] = kexgex_server;
	kex->kex[KEX_DH_GEX_SHA256] = kexgex_server;
	kex->kex[KEX_ECDH_SHA2] = kexecdh_server;
#endif
	kex->kex[KEX_C25519_SHA256] = kexc25519_server;
	kex->server = 1;
	kex->client_version_string=client_version_string;
	kex->server_version_string=server_version_string;
	kex->load_host_public_key=&get_hostkey_public_by_type;
	kex->load_host_private_key=&get_hostkey_private_by_type;
	kex->host_key_index=&get_hostkey_index;
	kex->sign = sshd_hostkey_sign;

	dispatch_run(DISPATCH_BLOCK, &kex->done, active_state);

	session_id2 = kex->session_id;
	session_id2_len = kex->session_id_len;

#ifdef DEBUG_KEXDH
	/* send 1st encrypted/maced/compressed message */
	packet_start(SSH2_MSG_IGNORE);
	packet_put_cstring("markus");
	packet_send();
	packet_write_wait();
#endif
	debug("KEX done");
}
