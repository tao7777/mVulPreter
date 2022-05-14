SAPI_API int sapi_header_op(sapi_header_op_enum op, void *arg TSRMLS_DC)
{
	sapi_header_struct sapi_header;
	char *colon_offset;
	char *header_line;
	uint header_line_len;
	int http_response_code;

	if (SG(headers_sent) && !SG(request_info).no_headers) {
		const char *output_start_filename = php_output_get_start_filename(TSRMLS_C);
		int output_start_lineno = php_output_get_start_lineno(TSRMLS_C);

		if (output_start_filename) {
			sapi_module.sapi_error(E_WARNING, "Cannot modify header information - headers already sent by (output started at %s:%d)",
				output_start_filename, output_start_lineno);
		} else {
			sapi_module.sapi_error(E_WARNING, "Cannot modify header information - headers already sent");
		}
		return FAILURE;
	}

	switch (op) {
		case SAPI_HEADER_SET_STATUS:
			sapi_update_response_code((int)(zend_intptr_t) arg TSRMLS_CC);
			return SUCCESS;

		case SAPI_HEADER_ADD:
		case SAPI_HEADER_REPLACE:
		case SAPI_HEADER_DELETE: {
				sapi_header_line *p = arg;

				if (!p->line || !p->line_len) {
					return FAILURE;
				}
				header_line = p->line;
				header_line_len = p->line_len;
				http_response_code = p->response_code;
				break;
			}

		case SAPI_HEADER_DELETE_ALL:
			if (sapi_module.header_handler) {
				sapi_module.header_handler(&sapi_header, op, &SG(sapi_headers) TSRMLS_CC);
			}
			zend_llist_clean(&SG(sapi_headers).headers);
			return SUCCESS;

		default:
			return FAILURE;
	}

	header_line = estrndup(header_line, header_line_len);

	/* cut off trailing spaces, linefeeds and carriage-returns */
	if (header_line_len && isspace(header_line[header_line_len-1])) {
		do {
			header_line_len--;
		} while(header_line_len && isspace(header_line[header_line_len-1]));
		header_line[header_line_len]='\0';
	}
	
	if (op == SAPI_HEADER_DELETE) {
		if (strchr(header_line, ':')) {
			efree(header_line);
			sapi_module.sapi_error(E_WARNING, "Header to delete may not contain colon.");
			return FAILURE;
		}
		if (sapi_module.header_handler) {
			sapi_header.header = header_line;
			sapi_header.header_len = header_line_len;
			sapi_module.header_handler(&sapi_header, op, &SG(sapi_headers) TSRMLS_CC);
		}
		sapi_remove_header(&SG(sapi_headers).headers, header_line, header_line_len);
		efree(header_line);
		return SUCCESS;
	} else {
 		/* new line/NUL character safety check */
 		int i;
 		for (i = 0; i < header_line_len; i++) {
			/* RFC 7230 ch. 3.2.4 deprecates folding support */
			if (header_line[i] == '\n' || header_line[i] == '\r') {
 				efree(header_line);
 				sapi_module.sapi_error(E_WARNING, "Header may not contain "
 						"more than a single header, new line detected");
				return FAILURE;
			}
			if (header_line[i] == '\0') {
				efree(header_line);
				sapi_module.sapi_error(E_WARNING, "Header may not contain NUL bytes");
				return FAILURE;
			}
		}
	}

	sapi_header.header = header_line;
	sapi_header.header_len = header_line_len;

	/* Check the header for a few cases that we have special support for in SAPI */
	if (header_line_len>=5 
		&& !strncasecmp(header_line, "HTTP/", 5)) {
		/* filter out the response code */
		sapi_update_response_code(sapi_extract_response_code(header_line) TSRMLS_CC);
		/* sapi_update_response_code doesn't free the status line if the code didn't change */
		if (SG(sapi_headers).http_status_line) {
			efree(SG(sapi_headers).http_status_line);
		}
		SG(sapi_headers).http_status_line = header_line;
		return SUCCESS;
	} else {
		colon_offset = strchr(header_line, ':');
		if (colon_offset) {
			*colon_offset = 0;
			if (!STRCASECMP(header_line, "Content-Type")) {
				char *ptr = colon_offset+1, *mimetype = NULL, *newheader;
				size_t len = header_line_len - (ptr - header_line), newlen;
				while (*ptr == ' ') {
					ptr++;
					len--;
				}

				/* Disable possible output compression for images */
				if (!strncmp(ptr, "image/", sizeof("image/")-1)) {
					zend_alter_ini_entry("zlib.output_compression", sizeof("zlib.output_compression"), "0", sizeof("0") - 1, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
				}

				mimetype = estrdup(ptr);
				newlen = sapi_apply_default_charset(&mimetype, len TSRMLS_CC);
				if (!SG(sapi_headers).mimetype){
					SG(sapi_headers).mimetype = estrdup(mimetype);
				}

				if (newlen != 0) {
					newlen += sizeof("Content-type: ");
					newheader = emalloc(newlen);
					PHP_STRLCPY(newheader, "Content-type: ", newlen, sizeof("Content-type: ")-1);
					strlcat(newheader, mimetype, newlen);
					sapi_header.header = newheader;
					sapi_header.header_len = newlen - 1;
					efree(header_line);
				}
				efree(mimetype);
				SG(sapi_headers).send_default_content_type = 0;
			} else if (!STRCASECMP(header_line, "Content-Length")) {
				/* Script is setting Content-length. The script cannot reasonably
				 * know the size of the message body after compression, so it's best
				 * do disable compression altogether. This contributes to making scripts
				 * portable between setups that have and don't have zlib compression
				 * enabled globally. See req #44164 */
				zend_alter_ini_entry("zlib.output_compression", sizeof("zlib.output_compression"),
					"0", sizeof("0") - 1, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
			} else if (!STRCASECMP(header_line, "Location")) {
				if ((SG(sapi_headers).http_response_code < 300 ||
					SG(sapi_headers).http_response_code > 399) &&
					SG(sapi_headers).http_response_code != 201) {
					/* Return a Found Redirect if one is not already specified */
					if (http_response_code) { /* user specified redirect code */
						sapi_update_response_code(http_response_code TSRMLS_CC);
					} else if (SG(request_info).proto_num > 1000 && 
					   SG(request_info).request_method && 
					   strcmp(SG(request_info).request_method, "HEAD") &&
					   strcmp(SG(request_info).request_method, "GET")) {
						sapi_update_response_code(303 TSRMLS_CC);
					} else {
						sapi_update_response_code(302 TSRMLS_CC);
					}
				}
			} else if (!STRCASECMP(header_line, "WWW-Authenticate")) { /* HTTP Authentication */
				sapi_update_response_code(401 TSRMLS_CC); /* authentication-required */
			}
			if (sapi_header.header==header_line) {
				*colon_offset = ':';
			}
		}
	}
	if (http_response_code) {
		sapi_update_response_code(http_response_code TSRMLS_CC);
	}
	sapi_header_add_op(op, &sapi_header TSRMLS_CC);
	return SUCCESS;
}
