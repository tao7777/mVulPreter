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
 
       /* We can always safely skip the explicit IV. We check at the beginning
        * of this function that the record has at least enough space for the
        * IV, MAC and padding length byte. (These can be checked in
        * non-constant time because it's all public information.) So, if the
        * padding was invalid, then we didn't change |rec->length| and this is
        * safe. If the padding was valid then we know that we have at least
        * overhead+padding_length bytes of space and so this is still safe
        * because overhead accounts for the explicit IV. */
       if (has_explicit_iv)
               {
               rec->data += block_size;
               rec->input += block_size;
               rec->length -= block_size;
               }

        padding_length = rec->data[rec->length-1];
 
        /* NB: if compression is in operation the first packet may not be of
			padding_length--;
			}
