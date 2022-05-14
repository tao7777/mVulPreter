int git_pkt_parse_line(
	git_pkt **head, const char *line, const char **out, size_t bufflen)
{
	int ret;
	int32_t len;

	/* Not even enough for the length */
	if (bufflen > 0 && bufflen < PKT_LEN_SIZE)
		return GIT_EBUFS;

	len = parse_len(line);
	if (len < 0) {
		/*
		 * If we fail to parse the length, it might be because the
		 * server is trying to send us the packfile already.
		 */
		if (bufflen >= 4 && !git__prefixcmp(line, "PACK")) {
			giterr_clear();
			*out = line;
			return pack_pkt(head);
		}

		return (int)len;
	}

	/*
	 * If we were given a buffer length, then make sure there is
	 * enough in the buffer to satisfy this line
	 */
 	if (bufflen > 0 && bufflen < (size_t)len)
 		return GIT_EBUFS;
 
 	line += PKT_LEN_SIZE;
 	/*
 	 * TODO: How do we deal with empty lines? Try again? with the next
	 * line?
	 */
	if (len == PKT_LEN_SIZE) {
		*head = NULL;
		*out = line;
		return 0;
	}

	if (len == 0) { /* Flush pkt */
		*out = line;
		return flush_pkt(head);
	}

	len -= PKT_LEN_SIZE; /* the encoded length includes its own size */

	if (*line == GIT_SIDE_BAND_DATA)
		ret = data_pkt(head, line, len);
	else if (*line == GIT_SIDE_BAND_PROGRESS)
		ret = sideband_progress_pkt(head, line, len);
	else if (*line == GIT_SIDE_BAND_ERROR)
		ret = sideband_error_pkt(head, line, len);
	else if (!git__prefixcmp(line, "ACK"))
		ret = ack_pkt(head, line, len);
	else if (!git__prefixcmp(line, "NAK"))
		ret = nak_pkt(head);
	else if (!git__prefixcmp(line, "ERR "))
		ret = err_pkt(head, line, len);
	else if (*line == '#')
		ret = comment_pkt(head, line, len);
	else if (!git__prefixcmp(line, "ok"))
		ret = ok_pkt(head, line, len);
	else if (!git__prefixcmp(line, "ng"))
		ret = ng_pkt(head, line, len);
	else if (!git__prefixcmp(line, "unpack"))
		ret = unpack_pkt(head, line, len);
	else
		ret = ref_pkt(head, line, len);

	*out = line + len;

	return ret;
}
