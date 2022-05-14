static void handle_data_packet(struct mt_connection *curconn, struct mt_mactelnet_hdr *pkthdr, int data_len) {
	struct mt_mactelnet_control_hdr cpkt;
	struct mt_packet pdata;
	unsigned char *data = pkthdr->data;
	unsigned int act_size = 0;
	int got_user_packet = 0;
	int got_pass_packet = 0;
	int got_width_packet = 0;
	int got_height_packet = 0;
	int success;

	/* Parse first control packet */
	success = parse_control_packet(data, data_len - MT_HEADER_LEN, &cpkt);

	while (success) {
		if (cpkt.cptype == MT_CPTYPE_BEGINAUTH) {
			int plen,i;
			if (!curconn->have_pass_salt) {
				for (i = 0; i < 16; ++i) {
					curconn->pass_salt[i] = rand() % 256;
				}
				curconn->have_pass_salt = 1;

				memset(curconn->trypassword, 0, sizeof(curconn->trypassword));
			}
			init_packet(&pdata, MT_PTYPE_DATA, pkthdr->dstaddr, pkthdr->srcaddr, pkthdr->seskey, curconn->outcounter);
			plen = add_control_packet(&pdata, MT_CPTYPE_PASSSALT, (curconn->pass_salt), 16);
			curconn->outcounter += plen;

			send_udp(curconn, &pdata);
		
		/* Don't change the username after the state is active */
		} else if (cpkt.cptype == MT_CPTYPE_USERNAME && curconn->state != STATE_ACTIVE) {
			memcpy(curconn->username, cpkt.data, act_size = (cpkt.length > MT_MNDP_MAX_STRING_SIZE - 1 ? MT_MNDP_MAX_STRING_SIZE - 1 : cpkt.length));
			curconn->username[act_size] = 0;
			got_user_packet = 1;

		} else if (cpkt.cptype == MT_CPTYPE_TERM_WIDTH && cpkt.length >= 2) {
			unsigned short width;

			memcpy(&width, cpkt.data, 2);
			curconn->terminal_width = le16toh(width);
			got_width_packet = 1;

		} else if (cpkt.cptype == MT_CPTYPE_TERM_HEIGHT && cpkt.length >= 2) {
			unsigned short height;

			memcpy(&height, cpkt.data, 2);
			curconn->terminal_height = le16toh(height);
			got_height_packet = 1;

		} else if (cpkt.cptype == MT_CPTYPE_TERM_TYPE) {

 			memcpy(curconn->terminal_type, cpkt.data, act_size = (cpkt.length > 30 - 1 ? 30 - 1 : cpkt.length));
 			curconn->terminal_type[act_size] = 0;
 
		} else if (cpkt.cptype == MT_CPTYPE_PASSWORD && cpkt.length == 17) {
 
 #if defined(__linux__) && defined(_POSIX_MEMLOCK_RANGE)
 			mlock(curconn->trypassword, 17);
#endif
			memcpy(curconn->trypassword, cpkt.data, 17);
			got_pass_packet = 1;

		} else if (cpkt.cptype == MT_CPTYPE_PLAINDATA) {

			/* relay data from client to shell */
			if (curconn->state == STATE_ACTIVE && curconn->ptsfd != -1) {
				write(curconn->ptsfd, cpkt.data, cpkt.length);
 			}
 
 		} else {
			syslog(LOG_WARNING, _("(%d) Unhandeled control packet type: %d, length: %d"), curconn->seskey, cpkt.cptype, cpkt.length);
 		}
 
 		/* Parse next control packet */
		success = parse_control_packet(NULL, 0, &cpkt);
	}

	if (got_user_packet && got_pass_packet) {
		user_login(curconn, pkthdr);
	}

	if (curconn->state == STATE_ACTIVE && (got_width_packet || got_height_packet)) {
		set_terminal_size(curconn->ptsfd, curconn->terminal_width, curconn->terminal_height);

	}
}
