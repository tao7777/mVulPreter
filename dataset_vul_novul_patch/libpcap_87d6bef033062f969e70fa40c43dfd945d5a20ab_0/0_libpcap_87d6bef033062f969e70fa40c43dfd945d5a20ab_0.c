read_bytes(FILE *fp, void *buf, size_t bytes_to_read, int fail_on_eof,
    char *errbuf)
{
	size_t amt_read;

	amt_read = fread(buf, 1, bytes_to_read, fp);
	if (amt_read != bytes_to_read) {
		if (ferror(fp)) {
			pcap_fmt_errmsg_for_errno(errbuf, PCAP_ERRBUF_SIZE,
			    errno, "error reading dump file");
		} else {
 			if (amt_read == 0 && !fail_on_eof)
 				return (0);	/* EOF */
 			pcap_snprintf(errbuf, PCAP_ERRBUF_SIZE,
			    "truncated pcapng dump file; tried to read %" PRIsize " bytes, only got %" PRIsize,
 			    bytes_to_read, amt_read);
 		}
 		return (-1);
	}
	return (1);
}
