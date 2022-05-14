parse_cosine_hex_dump(FILE_T fh, struct wtap_pkthdr *phdr, int pkt_len,
    Buffer* buf, int *err, gchar **err_info)
{
	guint8 *pd;
	gchar	line[COSINE_LINE_LENGTH];
	int	i, hex_lines, n, caplen = 0;
 	/* Make sure we have enough room for the packet */
	ws_buffer_assure_space(buf, COSINE_MAX_PACKET_LEN);
 	pd = ws_buffer_start_ptr(buf);
 
 	/* Calculate the number of hex dump lines, each
	 * containing 16 bytes of data */
	hex_lines = pkt_len / 16 + ((pkt_len % 16) ? 1 : 0);

	for (i = 0; i < hex_lines; i++) {
		if (file_gets(line, COSINE_LINE_LENGTH, fh) == NULL) {
			*err = file_error(fh, err_info);
			if (*err == 0) {
				*err = WTAP_ERR_SHORT_READ;
			}
			return FALSE;
		}
		if (empty_line(line)) {
			break;
		}
		if ((n = parse_single_hex_dump_line(line, pd, i*16)) == -1) {
			*err = WTAP_ERR_BAD_FILE;
			*err_info = g_strdup("cosine: hex dump line doesn't have 16 numbers");
			return FALSE;
		}
		caplen += n;
	}
	phdr->caplen = caplen;
 	return TRUE;
 }
