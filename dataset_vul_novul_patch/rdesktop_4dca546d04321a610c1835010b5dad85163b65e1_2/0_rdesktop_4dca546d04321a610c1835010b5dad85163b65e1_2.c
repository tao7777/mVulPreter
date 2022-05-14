cssp_read_tsrequest(STREAM token, STREAM pubkey)
{
 	STREAM s;
 	int length;
 	int tagval;
	struct stream packet;
 
 	s = tcp_recv(NULL, 4);
 
	if (s == NULL)
		return False;

	if (s->p[0] != (BER_TAG_SEQUENCE | BER_TAG_CONSTRUCTED))
	{
		logger(Protocol, Error,
		       "cssp_read_tsrequest(), expected BER_TAG_SEQUENCE|BER_TAG_CONSTRUCTED, got %x",
		       s->p[0]);
		return False;
	}

	if (s->p[1] < 0x80)
		length = s->p[1] - 2;
	else if (s->p[1] == 0x81)
		length = s->p[2] - 1;
	else if (s->p[1] == 0x82)
		length = (s->p[2] << 8) | s->p[3];
	else
		return False;
 
 	s = tcp_recv(s, length);
	packet = *s;
 
 	if (!ber_in_header(s, &tagval, &length) ||
	    tagval != (BER_TAG_SEQUENCE | BER_TAG_CONSTRUCTED))
		return False;

 	if (!ber_in_header(s, &tagval, &length) ||
 	    tagval != (BER_TAG_CTXT_SPECIFIC | BER_TAG_CONSTRUCTED | 0))
 		return False;

	if (!s_check_rem(s, length))
	{
		 rdp_protocol_error("cssp_read_tsrequest(), consume of version from stream would overrun",
				    &packet);
	}
 	in_uint8s(s, length);
 
	if (token)
	{
		if (!ber_in_header(s, &tagval, &length)
		    || tagval != (BER_TAG_CTXT_SPECIFIC | BER_TAG_CONSTRUCTED | 1))
			return False;
		if (!ber_in_header(s, &tagval, &length)
		    || tagval != (BER_TAG_SEQUENCE | BER_TAG_CONSTRUCTED))
			return False;
		if (!ber_in_header(s, &tagval, &length)
		    || tagval != (BER_TAG_SEQUENCE | BER_TAG_CONSTRUCTED))
			return False;
		if (!ber_in_header(s, &tagval, &length)
		    || tagval != (BER_TAG_CTXT_SPECIFIC | BER_TAG_CONSTRUCTED | 0))
			return False;

 		if (!ber_in_header(s, &tagval, &length) || tagval != BER_TAG_OCTET_STRING)
 			return False;
 
		if (!s_check_rem(s, length))
		{
			rdp_protocol_error("cssp_read_tsrequest(), consume of token from stream would overrun",
					   &packet);
		}

		s_realloc(token, length);
		s_reset(token);
 		out_uint8p(token, s->p, length);
 		s_mark_end(token);
 	}

	if (pubkey)
	{
		if (!ber_in_header(s, &tagval, &length)
		    || tagval != (BER_TAG_CTXT_SPECIFIC | BER_TAG_CONSTRUCTED | 3))
			return False;

		if (!ber_in_header(s, &tagval, &length) || tagval != BER_TAG_OCTET_STRING)
			return False;

		pubkey->data = pubkey->p = s->p;
		pubkey->end = pubkey->data + length;
		pubkey->size = length;
	}


	return True;
}
