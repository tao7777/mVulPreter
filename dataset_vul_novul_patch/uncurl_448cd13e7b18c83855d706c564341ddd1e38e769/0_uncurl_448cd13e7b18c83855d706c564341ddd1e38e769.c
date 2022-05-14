UNCURL_EXPORT int32_t uncurl_ws_accept(struct uncurl_conn *ucc, char **origins, int32_t n_origins)
{
	int32_t e;

	e = uncurl_read_header(ucc);
	if (e != UNCURL_OK) return e;

	uncurl_set_header_str(ucc, "Upgrade", "websocket");
	uncurl_set_header_str(ucc, "Connection", "Upgrade");

	char *origin = NULL;
 	e = uncurl_get_header_str(ucc, "Origin", &origin);
 	if (e != UNCURL_OK) return e;
 
	//the substring MUST came at the end of the origin header, thus a strstr AND a strcmp
 	bool origin_ok = false;
	for (int32_t x = 0; x < n_origins; x++) {
		char *match = strstr(origin, origins[x]);
		if (match && !strcmp(match, origins[x])) {origin_ok = true; break;}
	}
 
 	if (!origin_ok) return UNCURL_WS_ERR_ORIGIN;
 
	char *sec_key = NULL;
	e = uncurl_get_header_str(ucc, "Sec-WebSocket-Key", &sec_key);
	if (e != UNCURL_OK) return e;

	char *accept_key = ws_create_accept_key(sec_key);
	uncurl_set_header_str(ucc, "Sec-WebSocket-Accept", accept_key);
	free(accept_key);

	e = uncurl_write_header(ucc, "101", "Switching Protocols", UNCURL_RESPONSE);
	if (e != UNCURL_OK) return e;

	ucc->ws_mask = 0;

	return UNCURL_OK;
}
