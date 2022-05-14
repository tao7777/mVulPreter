 mcs_parse_domain_params(STREAM s)
 {
	uint32 length;
	struct stream packet = *s;
 
 	ber_parse_header(s, MCS_TAG_DOMAIN_PARAMS, &length);

	if (!s_check_rem(s, length))
	{
		rdp_protocol_error("mcs_parse_domain_params(), consume domain params from stream would overrun", &packet);
	}

 	in_uint8s(s, length);
 
 	return s_check(s);
}
