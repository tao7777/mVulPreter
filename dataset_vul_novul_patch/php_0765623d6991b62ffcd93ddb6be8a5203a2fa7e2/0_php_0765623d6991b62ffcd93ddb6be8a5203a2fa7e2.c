ftp_genlist(ftpbuf_t *ftp, const char *cmd, const char *path TSRMLS_DC)
{
	php_stream	*tmpstream = NULL;
	databuf_t	*data = NULL;
	char		*ptr;
	int		ch, lastch;
	size_t		size, rcvd;
	size_t		lines;
	char		**ret = NULL;
	char		**entry;
	char		*text;


	if ((tmpstream = php_stream_fopen_tmpfile()) == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to create temporary file.  Check permissions in temporary files directory.");
		return NULL;
	}

	if (!ftp_type(ftp, FTPTYPE_ASCII)) {
		goto bail;
	}

	if ((data = ftp_getdata(ftp TSRMLS_CC)) == NULL) {
		goto bail;
	}
	ftp->data = data;

	if (!ftp_putcmd(ftp, cmd, path)) {
		goto bail;
	}
	if (!ftp_getresp(ftp) || (ftp->resp != 150 && ftp->resp != 125 && ftp->resp != 226)) {
		goto bail;
	}

	/* some servers don't open a ftp-data connection if the directory is empty */
	if (ftp->resp == 226) {
		ftp->data = data_close(ftp, data);
		php_stream_close(tmpstream);
		return ecalloc(1, sizeof(char*));
	}

	/* pull data buffer into tmpfile */
	if ((data = data_accept(data, ftp TSRMLS_CC)) == NULL) {
		goto bail;
	}
	size = 0;
	lines = 0;
	lastch = 0;
	while ((rcvd = my_recv(ftp, data->fd, data->buf, FTP_BUFSIZE))) {
		if (rcvd == -1 || rcvd > ((size_t)(-1))-size) {
			goto bail;
		}

		php_stream_write(tmpstream, data->buf, rcvd);

		size += rcvd;
                for (ptr = data->buf; rcvd; rcvd--, ptr++) {
                        if (*ptr == '\n' && lastch == '\r') {
                                lines++;
                        }
                        lastch = *ptr;
                }
			lastch = *ptr;
		}
	}
