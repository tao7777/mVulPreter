parse_netscreen_rec_hdr(struct wtap_pkthdr *phdr, const char *line, char *cap_int,
    gboolean *cap_dir, char *cap_dst, int *err, gchar **err_info)
 {
	int	sec;
	int	dsec, pkt_len;
	char	direction[2];
	char	cap_src[13];
 
 	phdr->rec_type = REC_TYPE_PACKET;
 	phdr->presence_flags = WTAP_HAS_TS|WTAP_HAS_CAP_LEN;
 
	if (sscanf(line, "%9d.%9d: %15[a-z0-9/:.-](%1[io]) len=%9d:%12s->%12s/",
 		   &sec, &dsec, cap_int, direction, &pkt_len, cap_src, cap_dst) < 5) {
 		*err = WTAP_ERR_BAD_FILE;
 		*err_info = g_strdup("netscreen: Can't parse packet-header");
 		return -1;
 	}
 
	*cap_dir = (direction[0] == 'o' ? NETSCREEN_EGRESS : NETSCREEN_INGRESS);
 
 	phdr->ts.secs  = sec;
 	phdr->ts.nsecs = dsec * 100000000;
 	phdr->len = pkt_len;
 
	return pkt_len;
}
