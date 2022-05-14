static int decode_level3_header(LHAFileHeader **header, LHAInputStream *stream)
{
	unsigned int header_len;


	if (lha_decode_uint16(&RAW_DATA(header, 0)) != 4) {
		return 0;
	}


	if (!extend_raw_data(header, stream,
	                     LEVEL_3_HEADER_LEN - RAW_DATA_LEN(header))) {
		return 0;
	}

 
 	header_len = lha_decode_uint32(&RAW_DATA(header, 24));
 
	if (header_len > LEVEL_3_MAX_HEADER_LEN
	 || header_len < RAW_DATA_LEN(header)) {
 		return 0;
 	}
 
	if (!extend_raw_data(header, stream,
	                     header_len - RAW_DATA_LEN(header))) {
		return 0;
	}


	memcpy((*header)->compress_method, &RAW_DATA(header, 2), 5);
	(*header)->compress_method[5] = '\0';


	(*header)->compressed_length = lha_decode_uint32(&RAW_DATA(header, 7));
	(*header)->length = lha_decode_uint32(&RAW_DATA(header, 11));


	(*header)->timestamp = lha_decode_uint32(&RAW_DATA(header, 15));


	(*header)->crc = lha_decode_uint16(&RAW_DATA(header, 21));


	(*header)->os_type = RAW_DATA(header, 23);

	if (!decode_extended_headers(header, 28)) {
		return 0;
	}

	return 1;
}
