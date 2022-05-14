 mcs_parse_domain_params(STREAM s)
 {
	int length;
 
 	ber_parse_header(s, MCS_TAG_DOMAIN_PARAMS, &length);
 	in_uint8s(s, length);
 
 	return s_check(s);
}
