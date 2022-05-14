static void send_auth(char *username, char *password) {
	struct mt_packet data;
	unsigned short width = 0;
	unsigned short height = 0;
 	char *terminal = getenv("TERM");
 	char md5data[100];
 	unsigned char md5sum[17];
	int plen;
 	md5_state_t state;
 
 #if defined(__linux__) && defined(_POSIX_MEMLOCK_RANGE)
 	mlock(md5data, sizeof(md5data));
 	mlock(md5sum, sizeof(md5data));
 #endif
 
 	/* Concat string of 0 + password + pass_salt */
 	md5data[0] = 0;
	strncpy(md5data + 1, password, 82);
	md5data[83] = '\0';
	memcpy(md5data + 1 + strlen(password), pass_salt, 16);
 
 	/* Generate md5 sum of md5data with a leading 0 */
 	md5_init(&state);
	md5_append(&state, (const md5_byte_t *)md5data, strlen(password) + 17);
 	md5_finish(&state, (md5_byte_t *)md5sum + 1);
 	md5sum[0] = 0;
 
	/* Send combined packet to server */
	init_packet(&data, MT_PTYPE_DATA, srcmac, dstmac, sessionkey, outcounter);
	plen = add_control_packet(&data, MT_CPTYPE_PASSWORD, md5sum, 17);
	plen += add_control_packet(&data, MT_CPTYPE_USERNAME, username, strlen(username));
	plen += add_control_packet(&data, MT_CPTYPE_TERM_TYPE, terminal, strlen(terminal));
	
	if (is_a_tty && get_terminal_size(&width, &height) != -1) {
		width = htole16(width);
		height = htole16(height);
		plen += add_control_packet(&data, MT_CPTYPE_TERM_WIDTH, &width, 2);
		plen += add_control_packet(&data, MT_CPTYPE_TERM_HEIGHT, &height, 2);
	}

	outcounter += plen;

	/* TODO: handle result */
	send_udp(&data, 1);
}
