parse_cosine_rec_hdr(struct wtap_pkthdr *phdr, const char *line,
     int *err, gchar **err_info)
 {
 	union wtap_pseudo_header *pseudo_header = &phdr->pseudo_header;
 	int	num_items_scanned;
	int	yy, mm, dd, hr, min, sec, csec, pkt_len;
 	int	pro, off, pri, rm, error;
 	guint	code1, code2;
 	char	if_name[COSINE_MAX_IF_NAME_LEN] = "", direction[6] = "";
 	struct	tm tm;
 
 	if (sscanf(line, "%4d-%2d-%2d,%2d:%2d:%2d.%9d:",
 		   &yy, &mm, &dd, &hr, &min, &sec, &csec) == 7) {
 		/* appears to be output to a control blade */
 		num_items_scanned = sscanf(line,
		   "%4d-%2d-%2d,%2d:%2d:%2d.%9d: %5s (%127[A-Za-z0-9/:]), Length:%9d, Pro:%9d, Off:%9d, Pri:%9d, RM:%9d, Err:%9d [%8x, %8x]",
 			&yy, &mm, &dd, &hr, &min, &sec, &csec,
 				   direction, if_name, &pkt_len,
 				   &pro, &off, &pri, &rm, &error,
				   &code1, &code2);

 		if (num_items_scanned != 17) {
 			*err = WTAP_ERR_BAD_FILE;
 			*err_info = g_strdup("cosine: purported control blade line doesn't have code values");
			return -1;
 		}
 	} else {
 		/* appears to be output to PE */
 		num_items_scanned = sscanf(line,
		   "%5s (%127[A-Za-z0-9/:]), Length:%9d, Pro:%9d, Off:%9d, Pri:%9d, RM:%9d, Err:%9d [%8x, %8x]",
 				   direction, if_name, &pkt_len,
 				   &pro, &off, &pri, &rm, &error,
 				   &code1, &code2);
 
 		if (num_items_scanned != 10) {
 			*err = WTAP_ERR_BAD_FILE;
 			*err_info = g_strdup("cosine: header line is neither control blade nor PE output");
			return -1;
 		}
 		yy = mm = dd = hr = min = sec = csec = 0;
 	}
 
 	phdr->rec_type = REC_TYPE_PACKET;
 	phdr->presence_flags = WTAP_HAS_TS|WTAP_HAS_CAP_LEN;
	tm.tm_year = yy - 1900;
	tm.tm_mon = mm - 1;
	tm.tm_mday = dd;
	tm.tm_hour = hr;
	tm.tm_min = min;
	tm.tm_sec = sec;
	tm.tm_isdst = -1;
	phdr->ts.secs = mktime(&tm);
	phdr->ts.nsecs = csec * 10000000;
	phdr->len = pkt_len;

	/* XXX need to handle other encapsulations like Cisco HDLC,
	   Frame Relay and ATM */
	if (strncmp(if_name, "TEST:", 5) == 0) {
		pseudo_header->cosine.encap = COSINE_ENCAP_TEST;
	} else if (strncmp(if_name, "PPoATM:", 7) == 0) {
		pseudo_header->cosine.encap = COSINE_ENCAP_PPoATM;
	} else if (strncmp(if_name, "PPoFR:", 6) == 0) {
		pseudo_header->cosine.encap = COSINE_ENCAP_PPoFR;
	} else if (strncmp(if_name, "ATM:", 4) == 0) {
		pseudo_header->cosine.encap = COSINE_ENCAP_ATM;
	} else if (strncmp(if_name, "FR:", 3) == 0) {
		pseudo_header->cosine.encap = COSINE_ENCAP_FR;
	} else if (strncmp(if_name, "HDLC:", 5) == 0) {
		pseudo_header->cosine.encap = COSINE_ENCAP_HDLC;
	} else if (strncmp(if_name, "PPP:", 4) == 0) {
		pseudo_header->cosine.encap = COSINE_ENCAP_PPP;
	} else if (strncmp(if_name, "ETH:", 4) == 0) {
		pseudo_header->cosine.encap = COSINE_ENCAP_ETH;
	} else {
		pseudo_header->cosine.encap = COSINE_ENCAP_UNKNOWN;
	}
	if (strncmp(direction, "l2-tx", 5) == 0) {
		pseudo_header->cosine.direction = COSINE_DIR_TX;
	} else if (strncmp(direction, "l2-rx", 5) == 0) {
		pseudo_header->cosine.direction = COSINE_DIR_RX;
	}
	g_strlcpy(pseudo_header->cosine.if_name, if_name,
		COSINE_MAX_IF_NAME_LEN);
	pseudo_header->cosine.pro = pro;
	pseudo_header->cosine.off = off;
	pseudo_header->cosine.pri = pri;
 	pseudo_header->cosine.rm = rm;
 	pseudo_header->cosine.err = error;
 
	return pkt_len;
}
