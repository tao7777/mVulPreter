static bool handle_client_startup(PgSocket *client, PktHdr *pkt)
{
	const char *passwd;
	const uint8_t *key;
	bool ok;

	SBuf *sbuf = &client->sbuf;

	/* don't tolerate partial packets */
	if (incomplete_pkt(pkt)) {
		disconnect_client(client, true, "client sent partial pkt in startup phase");
		return false;
	}

	if (client->wait_for_welcome) {
		if  (finish_client_login(client)) {
			/* the packet was already parsed */
			sbuf_prepare_skip(sbuf, pkt->len);
			return true;
		} else
			return false;
	}

	switch (pkt->type) {
	case PKT_SSLREQ:
		slog_noise(client, "C: req SSL");
		slog_noise(client, "P: nak");

		/* reject SSL attempt */
		if (!sbuf_answer(&client->sbuf, "N", 1)) {
			disconnect_client(client, false, "failed to nak SSL");
			return false;
		}
		break;
	case PKT_STARTUP_V2:
		disconnect_client(client, true, "Old V2 protocol not supported");
		return false;
	case PKT_STARTUP:
		if (client->pool) {
			disconnect_client(client, true, "client re-sent startup pkt");
			return false;
		}

		if (!decide_startup_pool(client, pkt))
			return false;

		if (client->pool->db->admin) {
			if (!admin_pre_login(client))
				return false;
		}

		if (cf_auth_type <= AUTH_TRUST || client->own_user) {
			if (!finish_client_login(client))
				return false;
		} else {
			if (!send_client_authreq(client)) {
				disconnect_client(client, false, "failed to send auth req");
				return false;
			}
 		}
 		break;
 	case 'p':		/* PasswordMessage */
		/* too early */
		if (!client->auth_user) {
			disconnect_client(client, true, "client password pkt before startup packet");
			return false;
		}

 		/* haven't requested it */
 		if (cf_auth_type <= AUTH_TRUST) {
 			disconnect_client(client, true, "unrequested passwd pkt");
			return false;
		}

		ok = mbuf_get_string(&pkt->data, &passwd);
		if (ok && check_client_passwd(client, passwd)) {
			if (!finish_client_login(client))
				return false;
		} else {
			disconnect_client(client, true, "Auth failed");
			return false;
		}
		break;
	case PKT_CANCEL:
		if (mbuf_avail_for_read(&pkt->data) == BACKENDKEY_LEN
		    && mbuf_get_bytes(&pkt->data, BACKENDKEY_LEN, &key))
		{
			memcpy(client->cancel_key, key, BACKENDKEY_LEN);
			accept_cancel_request(client);
		} else
			disconnect_client(client, false, "bad cancel request");
		return false;
	default:
		disconnect_client(client, false, "bad packet");
		return false;
	}
	sbuf_prepare_skip(sbuf, pkt->len);
	client->request_time = get_cached_time();
	return true;
}
