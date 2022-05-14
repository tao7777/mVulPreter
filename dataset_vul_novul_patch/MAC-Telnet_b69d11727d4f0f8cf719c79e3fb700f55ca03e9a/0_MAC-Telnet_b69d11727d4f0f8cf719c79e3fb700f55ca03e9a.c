static int handle_packet(unsigned char *data, int data_len) {
	struct mt_mactelnet_hdr pkthdr;

	/* Minimal size checks (pings are not supported here) */
	if (data_len < MT_HEADER_LEN){
		return -1;
	}
	parse_packet(data, &pkthdr);

	/* We only care about packets with correct sessionkey */
	if (pkthdr.seskey != sessionkey) {
		return -1;
	}

	/* Handle data packets */
	if (pkthdr.ptype == MT_PTYPE_DATA) {
		struct mt_packet odata;
		struct mt_mactelnet_control_hdr cpkt;
		int success = 0;

		/* Always transmit ACKNOWLEDGE packets in response to DATA packets */
		init_packet(&odata, MT_PTYPE_ACK, srcmac, dstmac, sessionkey, pkthdr.counter + (data_len - MT_HEADER_LEN));
		send_udp(&odata, 0);

		/* Accept first packet, and all packets greater than incounter, and if counter has
		wrapped around. */
		if (pkthdr.counter > incounter || (incounter - pkthdr.counter) > 65535) {
			incounter = pkthdr.counter;
		} else {
			/* Ignore double or old packets */
			return -1;
		}

		/* Parse controlpacket data */
		success = parse_control_packet(data + MT_HEADER_LEN, data_len - MT_HEADER_LEN, &cpkt);

		while (success) {
 
 			/* If we receive pass_salt, transmit auth data back */
 			if (cpkt.cptype == MT_CPTYPE_PASSSALT) {
				/* check validity, server sends exactly 16 bytes */
				if (cpkt.length != 16) {
					fprintf(stderr, _("Invalid salt length: %d (instead of 16) received from server %s\n"), cpkt.length, ether_ntoa((struct ether_addr *)dstmac));
				}
				memcpy(pass_salt, cpkt.data, 16);
 				send_auth(username, password);
 			}
 
			/* If the (remaining) data did not have a control-packet magic byte sequence,
			   the data is raw terminal data to be outputted to the terminal. */
			else if (cpkt.cptype == MT_CPTYPE_PLAINDATA) {
				fwrite((const void *)cpkt.data, 1, cpkt.length, stdout);
			}

			/* END_AUTH means that the user/password negotiation is done, and after this point
			   terminal data may arrive, so we set up the terminal to raw mode. */
			else if (cpkt.cptype == MT_CPTYPE_END_AUTH) {

				/* we have entered "terminal mode" */
				terminal_mode = 1;

				if (is_a_tty) {
					/* stop input buffering at all levels. Give full control of terminal to RouterOS */
					raw_term();

					setvbuf(stdin,  (char*)NULL, _IONBF, 0);

					/* Add resize signal handler */
					signal(SIGWINCH, sig_winch);
				}
			}

			/* Parse next controlpacket */
			success = parse_control_packet(NULL, 0, &cpkt);
		}
	}
	else if (pkthdr.ptype == MT_PTYPE_ACK) {
		/* Handled elsewhere */
	}

	/* The server wants to terminate the connection, we have to oblige */
	else if (pkthdr.ptype == MT_PTYPE_END) {
		struct mt_packet odata;

		/* Acknowledge the disconnection by sending a END packet in return */
		init_packet(&odata, MT_PTYPE_END, srcmac, dstmac, pkthdr.seskey, 0);
		send_udp(&odata, 0);

		if (!quiet_mode) {
			fprintf(stderr, _("Connection closed.\n"));
		}

		/* exit */
		running = 0;
	} else {
		fprintf(stderr, _("Unhandeled packet type: %d received from server %s\n"), pkthdr.ptype, ether_ntoa((struct ether_addr *)dstmac));
		return -1;
	}

	return pkthdr.ptype;
}
