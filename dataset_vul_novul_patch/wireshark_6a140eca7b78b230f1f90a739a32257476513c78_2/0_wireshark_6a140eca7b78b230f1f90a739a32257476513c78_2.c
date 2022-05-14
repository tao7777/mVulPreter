parse_netscreen_rec_hdr(struct wtap_pkthdr *phdr, const char *line, char *cap_int,
static gboolean
parse_netscreen_packet(FILE_T fh, struct wtap_pkthdr *phdr, Buffer* buf,
    char *line, int *err, gchar **err_info)
 {
	int		sec;
	int		dsec;
	char		cap_int[NETSCREEN_MAX_INT_NAME_LENGTH];
	char		direction[2];
	guint		pkt_len;
	char		cap_src[13];
	char		cap_dst[13];
	guint8		*pd;
	gchar		*p;
	int		n, i = 0;
	guint		offset = 0;
	gchar		dststr[13];
 
 	phdr->rec_type = REC_TYPE_PACKET;
 	phdr->presence_flags = WTAP_HAS_TS|WTAP_HAS_CAP_LEN;
 
	if (sscanf(line, "%9d.%9d: %15[a-z0-9/:.-](%1[io]) len=%9u:%12s->%12s/",
 		   &sec, &dsec, cap_int, direction, &pkt_len, cap_src, cap_dst) < 5) {
 		*err = WTAP_ERR_BAD_FILE;
 		*err_info = g_strdup("netscreen: Can't parse packet-header");
 		return -1;
 	}
	if (pkt_len > WTAP_MAX_PACKET_SIZE) {
		/*
		 * Probably a corrupt capture file; don't blow up trying
		 * to allocate space for an immensely-large packet.
		 */
		*err = WTAP_ERR_BAD_FILE;
		*err_info = g_strdup_printf("netscreen: File has %u-byte packet, bigger than maximum of %u",
		    pkt_len, WTAP_MAX_PACKET_SIZE);
		return FALSE;
	}
 
	/*
	 * If direction[0] is 'o', the direction is NETSCREEN_EGRESS,
	 * otherwise it's NETSCREEN_INGRESS.
	 */
 
 	phdr->ts.secs  = sec;
 	phdr->ts.nsecs = dsec * 100000000;
 	phdr->len = pkt_len;
 
