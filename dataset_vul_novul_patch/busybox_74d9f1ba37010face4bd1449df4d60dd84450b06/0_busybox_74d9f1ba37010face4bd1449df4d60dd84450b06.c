uint8_t* FAST_FUNC udhcp_get_option32(struct dhcp_packet *packet, int code)
 {
 	uint8_t *r = udhcp_get_option(packet, code);
 	if (r) {
		if (r[-OPT_DATA + OPT_LEN] != 4)
 			r = NULL;
 	}
 	return r;
}
