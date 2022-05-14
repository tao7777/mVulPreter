 next_line(struct archive_read *a,
     const char **b, ssize_t *avail, ssize_t *ravail, ssize_t *nl)
{
	ssize_t len;
	int quit;
	
	quit = 0;
	if (*avail == 0) {
		*nl = 0;
		len = 0;
	} else
		len = get_line_size(*b, *avail, nl);
	/*
	 * Read bytes more while it does not reach the end of line.
	 */
	while (*nl == 0 && len == *avail && !quit) {
		ssize_t diff = *ravail - *avail;
		size_t nbytes_req = (*ravail+1023) & ~1023U;
		ssize_t tested;

		/* Increase reading bytes if it is not enough to at least
		 * new two lines. */
		if (nbytes_req < (size_t)*ravail + 160)
			nbytes_req <<= 1;

		*b = __archive_read_ahead(a, nbytes_req, avail);
		if (*b == NULL) {
			if (*ravail >= *avail)
				return (0);
			/* Reading bytes reaches the end of file. */
			*b = __archive_read_ahead(a, *avail, avail);
			quit = 1;
		}
		*ravail = *avail;
 		*b += diff;
 		*avail -= diff;
 		tested = len;/* Skip some bytes we already determinated. */
		len = get_line_size(*b + len, *avail - len, nl);
 		if (len >= 0)
 			len += tested;
 	}
	return (len);
}
