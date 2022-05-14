static void start_auth_request(PgSocket *client, const char *username)
{
 	int res;
 	PktBuf *buf;
 
 	/* have to fetch user info from db */
 	client->pool = get_pool(client->db, client->db->auth_user);
 	if (!find_server(client)) {
		client->wait_for_user_conn = true;
		return;
	}
	slog_noise(client, "Doing auth_conn query");
	client->wait_for_user_conn = false;
	client->wait_for_user = true;
	if (!sbuf_pause(&client->sbuf)) {
		release_server(client->link);
		disconnect_client(client, true, "pause failed");
		return;
	}
	client->link->ready = 0;

	res = 0;
	buf = pktbuf_dynamic(512);
	if (buf) {
		pktbuf_write_ExtQuery(buf, cf_auth_query, 1, username);
		res = pktbuf_send_immediate(buf, client->link);
		pktbuf_free(buf);
		/*
		 * Should do instead:
		 *   res = pktbuf_send_queued(buf, client->link);
		 * but that needs better integration with SBuf.
		 */
	}
	if (!res)
		disconnect_server(client->link, false, "unable to send login query");
}
