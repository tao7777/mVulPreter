int tls1_cbc_remove_padding(const SSL* s,
			    SSL3_RECORD *rec,
			    unsigned block_size,
			    unsigned mac_size)
	{
	unsigned padding_length, good, to_check, i;
	const char has_explicit_iv =
		s->version >= TLS1_1_VERSION || s->version == DTLS1_VERSION;
	const unsigned overhead = 1 /* padding length byte */ +
				  mac_size +
				  (has_explicit_iv ? block_size : 0);

	/* These lengths are all public so we can test them in non-constant
	 * time. */
        if (overhead > rec->length)
                return 0;
 
        padding_length = rec->data[rec->length-1];
 
        /* NB: if compression is in operation the first packet may not be of
			padding_length--;
			}
