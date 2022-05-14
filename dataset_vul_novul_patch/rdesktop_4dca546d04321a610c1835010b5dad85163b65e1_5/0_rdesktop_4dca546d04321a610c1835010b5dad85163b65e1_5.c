mcs_recv_connect_response(STREAM mcs_data)
 {
 	UNUSED(mcs_data);
 	uint8 result;
	uint32 length;
 	STREAM s;
	struct stream packet;
 	RD_BOOL is_fastpath;
 	uint8 fastpath_hdr;
 
	logger(Protocol, Debug, "%s()", __func__);
	s = iso_recv(&is_fastpath, &fastpath_hdr);
 
 	if (s == NULL)
 		return False;
	
	packet = *s;
 
 	ber_parse_header(s, MCS_CONNECT_RESPONSE, &length);
 
	ber_parse_header(s, BER_TAG_RESULT, &length);
	in_uint8(s, result);
	if (result != 0)
	{
		logger(Protocol, Error, "mcs_recv_connect_response(), result=%d", result);
		return False;
	}
 
 	ber_parse_header(s, BER_TAG_INTEGER, &length);
 	in_uint8s(s, length);	/* connect id */

	if (!s_check_rem(s, length))
	{
		rdp_protocol_error("mcs_recv_connect_response(), consume connect id from stream would overrun", &packet);
	}

 	mcs_parse_domain_params(s);
 
 	ber_parse_header(s, BER_TAG_OCTET_STRING, &length);

	sec_process_mcs_data(s);
	/*
	   if (length > mcs_data->size)
	   {
	   logger(Protocol, Error, "mcs_recv_connect_response(), expected length=%d, got %d",length, mcs_data->size);
	   length = mcs_data->size;
	   }

	   in_uint8a(s, mcs_data->data, length);
	   mcs_data->p = mcs_data->data;
	   mcs_data->end = mcs_data->data + length;
	 */
	return s_check_end(s);
}
