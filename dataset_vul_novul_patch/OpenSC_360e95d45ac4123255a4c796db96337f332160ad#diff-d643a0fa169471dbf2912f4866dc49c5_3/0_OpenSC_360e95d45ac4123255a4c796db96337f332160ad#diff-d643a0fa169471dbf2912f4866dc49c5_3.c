static int tcos_select_file(sc_card_t *card,
			    const sc_path_t *in_path,
			    sc_file_t **file_out)
{
	sc_context_t *ctx;
	sc_apdu_t apdu;
	sc_file_t *file=NULL;
	u8 buf[SC_MAX_APDU_BUFFER_SIZE], pathbuf[SC_MAX_PATH_SIZE], *path = pathbuf;
	unsigned int i;
	int r, pathlen;

	assert(card != NULL && in_path != NULL);
	ctx=card->ctx;
	memcpy(path, in_path->value, in_path->len);
	pathlen = in_path->len;

	sc_format_apdu(card, &apdu, SC_APDU_CASE_4_SHORT, 0xA4, 0, 0x04);
	
	switch (in_path->type) {
	case SC_PATH_TYPE_FILE_ID:
		if (pathlen != 2) return SC_ERROR_INVALID_ARGUMENTS;
		/* fall through */
	case SC_PATH_TYPE_FROM_CURRENT:
		apdu.p1 = 9;
		break;
	case SC_PATH_TYPE_DF_NAME:
		apdu.p1 = 4;
		break;
	case SC_PATH_TYPE_PATH:
		apdu.p1 = 8;
		if (pathlen >= 2 && memcmp(path, "\x3F\x00", 2) == 0) path += 2, pathlen -= 2;
		if (pathlen == 0) apdu.p1 = 0;
		break;
	case SC_PATH_TYPE_PARENT:
		apdu.p1 = 3;
		pathlen = 0;
		break;
	default:
		SC_FUNC_RETURN(ctx, SC_LOG_DEBUG_VERBOSE, SC_ERROR_INVALID_ARGUMENTS);
	}
	if( pathlen == 0 ) apdu.cse = SC_APDU_CASE_2_SHORT;

	apdu.lc = pathlen;
	apdu.data = path;
	apdu.datalen = pathlen;

	if (file_out != NULL) {
		apdu.resp = buf;
		apdu.resplen = sizeof(buf);
		apdu.le = 256;
	} else {
		apdu.resplen = 0;
		apdu.le = 0; 
		apdu.p2 = 0x0C; 
		apdu.cse = (pathlen == 0) ? SC_APDU_CASE_1 : SC_APDU_CASE_3_SHORT;
	}

	r = sc_transmit_apdu(card, &apdu);
	SC_TEST_RET(ctx, SC_LOG_DEBUG_NORMAL, r, "APDU transmit failed");
	r = sc_check_sw(card, apdu.sw1, apdu.sw2);
	if (r || file_out == NULL) SC_FUNC_RETURN(ctx, SC_LOG_DEBUG_VERBOSE, r);

	if (apdu.resplen < 1 || apdu.resp[0] != 0x62){
		sc_debug(ctx, SC_LOG_DEBUG_NORMAL, "received invalid template %02X\n", apdu.resp[0]);
		SC_FUNC_RETURN(ctx, SC_LOG_DEBUG_VERBOSE, SC_ERROR_UNKNOWN_DATA_RECEIVED);
	}

	file = sc_file_new();
	if (file == NULL) SC_FUNC_RETURN(ctx, SC_LOG_DEBUG_NORMAL, SC_ERROR_OUT_OF_MEMORY);
	*file_out = file;
 	file->path = *in_path;
 
 	for(i=2; i+1<apdu.resplen && i+1+apdu.resp[i+1]<apdu.resplen; i+=2+apdu.resp[i+1]){
		size_t j, len=apdu.resp[i+1];
 		unsigned char type=apdu.resp[i], *d=apdu.resp+i+2;
 
 		switch (type) {
		case 0x80:
		case 0x81:
			file->size=0;
			for(j=0; j<len; ++j) file->size = (file->size<<8) | d[j];
			break;
		case 0x82:
			file->shareable = (d[0] & 0x40) ? 1 : 0;
			file->ef_structure = d[0] & 7;
			switch ((d[0]>>3) & 7) {
			case 0: file->type = SC_FILE_TYPE_WORKING_EF; break;
			case 7: file->type = SC_FILE_TYPE_DF; break;
			default:
				sc_debug(ctx, SC_LOG_DEBUG_NORMAL, "invalid file type %02X in file descriptor\n", d[0]);
				SC_FUNC_RETURN(ctx, SC_LOG_DEBUG_VERBOSE, SC_ERROR_UNKNOWN_DATA_RECEIVED);
			}
			break;
		case 0x83:
 			file->id = (d[0]<<8) | d[1];
 			break;
 		case 0x84:
			file->namelen = MIN(sizeof file->name, len);
			memcpy(file->name, d, file->namelen);
 			break;
 		case 0x86:
 			sc_file_set_sec_attr(file, d, len); 
			break;
		default:
			if (len>0) sc_file_set_prop_attr(file, d, len); 
		}
	}
	file->magic = SC_FILE_MAGIC;

	parse_sec_attr(card, file, file->sec_attr, file->sec_attr_len);

	return 0;
}
