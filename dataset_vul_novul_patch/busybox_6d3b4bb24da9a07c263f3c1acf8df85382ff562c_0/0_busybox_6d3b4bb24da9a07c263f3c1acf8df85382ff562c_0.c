 static uint32_t select_lease_time(struct dhcp_packet *packet)
 {
 	uint32_t lease_time_sec = server_config.max_lease_sec;
	uint8_t *lease_time_opt = udhcp_get_option32(packet, DHCP_LEASE_TIME);
 	if (lease_time_opt) {
 		move_from_unaligned32(lease_time_sec, lease_time_opt);
 		lease_time_sec = ntohl(lease_time_sec);
		if (lease_time_sec > server_config.max_lease_sec)
			lease_time_sec = server_config.max_lease_sec;
		if (lease_time_sec < server_config.min_lease_sec)
			lease_time_sec = server_config.min_lease_sec;
	}
	return lease_time_sec;
}
