static uint8_t *extend_raw_data(LHAFileHeader **header,
                                LHAInputStream *stream,
                                size_t nbytes)
{
	LHAFileHeader *new_header;
 	size_t new_raw_len;
 	uint8_t *result;
 
	if (nbytes > LEVEL_3_MAX_HEADER_LEN) {
		return NULL;
	}

 
 	new_raw_len = RAW_DATA_LEN(header) + nbytes;
	new_header = realloc(*header, sizeof(LHAFileHeader) + new_raw_len);

	if (new_header == NULL) {
		return NULL;
	}


	*header = new_header;
	new_header->raw_data = (uint8_t *) (new_header + 1);
	result = new_header->raw_data + new_header->raw_data_len;


	if (!lha_input_stream_read(stream, result, nbytes)) {
		return NULL;
	}

	new_header->raw_data_len = new_raw_len;

	return result;
}
