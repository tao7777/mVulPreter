static char **fill_envp(struct dhcp_packet *packet)
{
	int envc;
	int i;
	char **envp, **curr;
	const char *opt_name;
	uint8_t *temp;
	uint8_t overload = 0;

#define BITMAP unsigned
#define BBITS (sizeof(BITMAP) * 8)
#define BMASK(i) (1 << (i & (sizeof(BITMAP) * 8 - 1)))
#define FOUND_OPTS(i) (found_opts[(unsigned)i / BBITS])
	BITMAP found_opts[256 / BBITS];

	memset(found_opts, 0, sizeof(found_opts));

	/* We need 6 elements for:
	 * "interface=IFACE"
	 * "ip=N.N.N.N" from packet->yiaddr
	 * "siaddr=IP" from packet->siaddr_nip (unless 0)
	 * "boot_file=FILE" from packet->file (unless overloaded)
	 * "sname=SERVER_HOSTNAME" from packet->sname (unless overloaded)
	 * terminating NULL
	 */
	envc = 6;
	/* +1 element for each option, +2 for subnet option: */
	if (packet) {
		/* note: do not search for "pad" (0) and "end" (255) options */
		for (i = 1; i < 255; i++) {
			temp = udhcp_get_option(packet, i);
			if (temp) {
				if (i == DHCP_OPTION_OVERLOAD)
					overload |= *temp;
				else if (i == DHCP_SUBNET)
					envc++; /* for $mask */
				envc++;
				/*if (i != DHCP_MESSAGE_TYPE)*/
				FOUND_OPTS(i) |= BMASK(i);
			}
		}
	}
	curr = envp = xzalloc(sizeof(envp[0]) * envc);

	*curr = xasprintf("interface=%s", client_config.interface);
	putenv(*curr++);

	if (!packet)
		return envp;

	/* Export BOOTP fields. Fields we don't (yet?) export:
	 * uint8_t op;      // always BOOTREPLY
	 * uint8_t htype;   // hardware address type. 1 = 10mb ethernet
	 * uint8_t hlen;    // hardware address length
	 * uint8_t hops;    // used by relay agents only
	 * uint32_t xid;
	 * uint16_t secs;   // elapsed since client began acquisition/renewal
	 * uint16_t flags;  // only one flag so far: bcast. Never set by server
	 * uint32_t ciaddr; // client IP (usually == yiaddr. can it be different
	 *                  // if during renew server wants to give us different IP?)
	 * uint32_t gateway_nip; // relay agent IP address
	 * uint8_t chaddr[16]; // link-layer client hardware address (MAC)
	 * TODO: export gateway_nip as $giaddr?
	 */
	/* Most important one: yiaddr as $ip */
	*curr = xmalloc(sizeof("ip=255.255.255.255"));
	sprint_nip(*curr, "ip=", (uint8_t *) &packet->yiaddr);
	putenv(*curr++);
	if (packet->siaddr_nip) {
		/* IP address of next server to use in bootstrap */
		*curr = xmalloc(sizeof("siaddr=255.255.255.255"));
		sprint_nip(*curr, "siaddr=", (uint8_t *) &packet->siaddr_nip);
		putenv(*curr++);
	}
	if (!(overload & FILE_FIELD) && packet->file[0]) {
		/* watch out for invalid packets */
		*curr = xasprintf("boot_file=%."DHCP_PKT_FILE_LEN_STR"s", packet->file);
		putenv(*curr++);
	}
	if (!(overload & SNAME_FIELD) && packet->sname[0]) {
		/* watch out for invalid packets */
		*curr = xasprintf("sname=%."DHCP_PKT_SNAME_LEN_STR"s", packet->sname);
		putenv(*curr++);
	}

	/* Export known DHCP options */
	opt_name = dhcp_option_strings;
	i = 0;
	while (*opt_name) {
		uint8_t code = dhcp_optflags[i].code;
		BITMAP *found_ptr = &FOUND_OPTS(code);
		BITMAP found_mask = BMASK(code);
		if (!(*found_ptr & found_mask))
			goto next;
		*found_ptr &= ~found_mask; /* leave only unknown options */
 		temp = udhcp_get_option(packet, code);
 		*curr = xmalloc_optname_optval(temp, &dhcp_optflags[i], opt_name);
 		putenv(*curr++);
		if (code == DHCP_SUBNET && temp[-OPT_DATA + OPT_LEN] == 4) {
 			/* Subnet option: make things like "$ip/$mask" possible */
 			uint32_t subnet;
 			move_from_unaligned32(subnet, temp);
			*curr = xasprintf("mask=%u", mton(subnet));
			putenv(*curr++);
		}
 next:
		opt_name += strlen(opt_name) + 1;
		i++;
	}
	/* Export unknown options */
	for (i = 0; i < 256;) {
		BITMAP bitmap = FOUND_OPTS(i);
		if (!bitmap) {
			i += BBITS;
			continue;
		}
		if (bitmap & BMASK(i)) {
			unsigned len, ofs;

			temp = udhcp_get_option(packet, i);
			/* udhcp_get_option returns ptr to data portion,
			 * need to go back to get len
			 */
			len = temp[-OPT_DATA + OPT_LEN];
			*curr = xmalloc(sizeof("optNNN=") + 1 + len*2);
			ofs = sprintf(*curr, "opt%u=", i);
			*bin2hex(*curr + ofs, (void*) temp, len) = '\0';
			putenv(*curr++);
		}
		i++;
	}

	return envp;
}
