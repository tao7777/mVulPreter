ssl_do_connect (server * serv)
{
	char buf[128];

	g_sess = serv->server_session;
	if (SSL_connect (serv->ssl) <= 0)
	{
		char err_buf[128];
		int err;

		g_sess = NULL;
		if ((err = ERR_get_error ()) > 0)
		{
			ERR_error_string (err, err_buf);
			snprintf (buf, sizeof (buf), "(%d) %s", err, err_buf);
			EMIT_SIGNAL (XP_TE_CONNFAIL, serv->server_session, buf, NULL,
							 NULL, NULL, 0);

			if (ERR_GET_REASON (err) == SSL_R_WRONG_VERSION_NUMBER)
				PrintText (serv->server_session, _("Are you sure this is a SSL capable server and port?\n"));

			server_cleanup (serv);

			if (prefs.hex_net_auto_reconnectonfail)
				auto_reconnect (serv, FALSE, -1);

			return (0);				  /* remove it (0) */
		}
	}
	g_sess = NULL;

	if (SSL_is_init_finished (serv->ssl))
	{
		struct cert_info cert_info;
		struct chiper_info *chiper_info;
		int verify_error;
		int i;

		if (!_SSL_get_cert_info (&cert_info, serv->ssl))
		{
			snprintf (buf, sizeof (buf), "* Certification info:");
			EMIT_SIGNAL (XP_TE_SSLMESSAGE, serv->server_session, buf, NULL, NULL,
							 NULL, 0);
			snprintf (buf, sizeof (buf), "  Subject:");
			EMIT_SIGNAL (XP_TE_SSLMESSAGE, serv->server_session, buf, NULL, NULL,
							 NULL, 0);
			for (i = 0; cert_info.subject_word[i]; i++)
			{
				snprintf (buf, sizeof (buf), "    %s", cert_info.subject_word[i]);
				EMIT_SIGNAL (XP_TE_SSLMESSAGE, serv->server_session, buf, NULL, NULL,
								 NULL, 0);
			}
			snprintf (buf, sizeof (buf), "  Issuer:");
			EMIT_SIGNAL (XP_TE_SSLMESSAGE, serv->server_session, buf, NULL, NULL,
							 NULL, 0);
			for (i = 0; cert_info.issuer_word[i]; i++)
			{
				snprintf (buf, sizeof (buf), "    %s", cert_info.issuer_word[i]);
				EMIT_SIGNAL (XP_TE_SSLMESSAGE, serv->server_session, buf, NULL, NULL,
								 NULL, 0);
			}
			snprintf (buf, sizeof (buf), "  Public key algorithm: %s (%d bits)",
						 cert_info.algorithm, cert_info.algorithm_bits);
			EMIT_SIGNAL (XP_TE_SSLMESSAGE, serv->server_session, buf, NULL, NULL,
							 NULL, 0);
			/*if (cert_info.rsa_tmp_bits)
			{
				snprintf (buf, sizeof (buf),
							 "  Public key algorithm uses ephemeral key with %d bits",
							 cert_info.rsa_tmp_bits);
				EMIT_SIGNAL (XP_TE_SSLMESSAGE, serv->server_session, buf, NULL, NULL,
								 NULL, 0);
			}*/
			snprintf (buf, sizeof (buf), "  Sign algorithm %s",
						 cert_info.sign_algorithm/*, cert_info.sign_algorithm_bits*/);
			EMIT_SIGNAL (XP_TE_SSLMESSAGE, serv->server_session, buf, NULL, NULL,
							 NULL, 0);
			snprintf (buf, sizeof (buf), "  Valid since %s to %s",
						 cert_info.notbefore, cert_info.notafter);
			EMIT_SIGNAL (XP_TE_SSLMESSAGE, serv->server_session, buf, NULL, NULL,
							 NULL, 0);
		} else
		{
			snprintf (buf, sizeof (buf), " * No Certificate");
			EMIT_SIGNAL (XP_TE_SSLMESSAGE, serv->server_session, buf, NULL, NULL,
							 NULL, 0);
		}

		chiper_info = _SSL_get_cipher_info (serv->ssl);	/* static buffer */
		snprintf (buf, sizeof (buf), "* Cipher info:");
		EMIT_SIGNAL (XP_TE_SSLMESSAGE, serv->server_session, buf, NULL, NULL, NULL,
						 0);
		snprintf (buf, sizeof (buf), "  Version: %s, cipher %s (%u bits)",
					 chiper_info->version, chiper_info->chiper,
					 chiper_info->chiper_bits);
		EMIT_SIGNAL (XP_TE_SSLMESSAGE, serv->server_session, buf, NULL, NULL, NULL,
						 0);

		verify_error = SSL_get_verify_result (serv->ssl);
 		switch (verify_error)
 		{
 		case X509_V_OK:
			{
				X509 *cert = SSL_get_peer_certificate (serv->ssl);
				int hostname_err;
				if ((hostname_err = _SSL_check_hostname(cert, serv->hostname)) != 0)
				{
					snprintf (buf, sizeof (buf), "* Verify E: Failed to validate hostname? (%d)%s",
							 hostname_err, serv->accept_invalid_cert ? " -- Ignored" : "");
					if (serv->accept_invalid_cert)
						EMIT_SIGNAL (XP_TE_SSLMESSAGE, serv->server_session, buf, NULL, NULL, NULL, 0);
					else
						goto conn_fail;
				}
				break;
			}
 			/* snprintf (buf, sizeof (buf), "* Verify OK (?)"); */
 			/* EMIT_SIGNAL (XP_TE_SSLMESSAGE, serv->server_session, buf, NULL, NULL, NULL, 0); */
 		case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY:
 		case X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE:
 		case X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT:
		case X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN:
		case X509_V_ERR_CERT_HAS_EXPIRED:
			if (serv->accept_invalid_cert)
			{
				snprintf (buf, sizeof (buf), "* Verify E: %s.? (%d) -- Ignored",
							 X509_verify_cert_error_string (verify_error),
							 verify_error);
				EMIT_SIGNAL (XP_TE_SSLMESSAGE, serv->server_session, buf, NULL, NULL,
								 NULL, 0);
				break;
			}
		default:
 			snprintf (buf, sizeof (buf), "%s.? (%d)",
 						 X509_verify_cert_error_string (verify_error),
 						 verify_error);
conn_fail:
 			EMIT_SIGNAL (XP_TE_CONNFAIL, serv->server_session, buf, NULL, NULL,
 							 NULL, 0);
 
			server_cleanup (serv);

			return (0);
		}

		server_stopconnecting (serv);

		/* activate gtk poll */
		server_connected (serv);

		return (0);					  /* remove it (0) */
	} else
	{
		if (serv->ssl->session && serv->ssl->session->time + SSLTMOUT < time (NULL))
		{
			snprintf (buf, sizeof (buf), "SSL handshake timed out");
			EMIT_SIGNAL (XP_TE_CONNFAIL, serv->server_session, buf, NULL,
							 NULL, NULL, 0);
			server_cleanup (serv); /* ->connecting = FALSE */

			if (prefs.hex_net_auto_reconnectonfail)
				auto_reconnect (serv, FALSE, -1);

			return (0);				  /* remove it (0) */
		}

		return (1);					  /* call it more (1) */
	}
}
