static int _server_handle_vCont(libgdbr_t *g, int (*cmd_cb) (void*, const char*, char*, size_t), void *core_ptr) {
	char *action = NULL;
	if (send_ack (g) < 0) {
		return -1;
	}
	g->data[g->data_len] = '\0';
	if (g->data[5] == '?') {
		return send_msg (g, "vCont;c;s");
	}
	if (!(action = strtok (g->data, ";"))) {
		return send_msg (g, "E01");
	}
	while (action = strtok (NULL, ";")) {
		eprintf ("action: %s\n", action);
		switch (action[0]) {
		case 's':
			if (cmd_cb (core_ptr, "ds", NULL, 0) < 0) {
				send_msg (g, "E01");
				return -1;
			}
			return send_msg (g, "OK");
		case 'c':
			if (cmd_cb (core_ptr, "dc", NULL, 0) < 0) {
				send_msg (g, "E01");
				return -1;
			}
			return send_msg (g, "OK");
		default:
 			return send_msg (g, "E01");
 		}
 	}
 }
