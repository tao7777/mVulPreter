static int http_RecvPostMessage(
	/*! HTTP Parser object. */
	http_parser_t *parser,
	/*! [in] Socket Information object. */
	SOCKINFO *info,
	/*! File where received data is copied to. */
	char *filename,
	/*! Send Instruction object which gives information whether the file
	 * is a virtual file or not. */
	struct SendInstruction *Instr)
{
	size_t Data_Buf_Size = 1024;
	char Buf[1024];
	int Timeout = -1;
	FILE *Fp;
	parse_status_t status = PARSE_OK;
	int ok_on_close = FALSE;
	size_t entity_offset = 0;
	int num_read = 0;
	int ret_code = HTTP_OK;

	if (Instr && Instr->IsVirtualFile) {
		Fp = (virtualDirCallback.open) (filename, UPNP_WRITE);
 		if (Fp == NULL)
 			return HTTP_INTERNAL_SERVER_ERROR;
 	} else {
#ifdef UPNP_ENABLE_POST_WRITE
 		Fp = fopen(filename, "wb");
 		if (Fp == NULL)
 			return HTTP_UNAUTHORIZED;
#else
		return HTTP_NOT_FOUND;
#endif
 	}
 	parser->position = POS_ENTITY;
 	do {
		/* first parse what has already been gotten */
		if (parser->position != POS_COMPLETE)
			status = parser_parse_entity(parser);
		if (status == PARSE_INCOMPLETE_ENTITY) {
			/* read until close */
			ok_on_close = TRUE;
		} else if ((status != PARSE_SUCCESS)
			   && (status != PARSE_CONTINUE_1)
			   && (status != PARSE_INCOMPLETE)) {
			/* error */
			ret_code = HTTP_BAD_REQUEST;
			goto ExitFunction;
		}
		/* read more if necessary entity */
		while (entity_offset + Data_Buf_Size > parser->msg.entity.length &&
		       parser->position != POS_COMPLETE) {
			num_read = sock_read(info, Buf, sizeof(Buf), &Timeout);
			if (num_read > 0) {
				/* append data to buffer */
				if (membuffer_append(&parser->msg.msg,
					Buf, (size_t)num_read) != 0) {
					/* set failure status */
					parser->http_error_code =
					    HTTP_INTERNAL_SERVER_ERROR;
					ret_code = HTTP_INTERNAL_SERVER_ERROR;
					goto ExitFunction;
				}
				status = parser_parse_entity(parser);
				if (status == PARSE_INCOMPLETE_ENTITY) {
					/* read until close */
					ok_on_close = TRUE;
				} else if ((status != PARSE_SUCCESS)
					   && (status != PARSE_CONTINUE_1)
					   && (status != PARSE_INCOMPLETE)) {
					ret_code = HTTP_BAD_REQUEST;
					goto ExitFunction;
				}
			} else if (num_read == 0) {
				if (ok_on_close) {
					UpnpPrintf(UPNP_INFO, HTTP, __FILE__, __LINE__,
						"<<< (RECVD) <<<\n%s\n-----------------\n",
						parser->msg.msg.buf);
					print_http_headers(&parser->msg);
					parser->position = POS_COMPLETE;
				} else {
					/* partial msg or response */
					parser->http_error_code = HTTP_BAD_REQUEST;
					ret_code = HTTP_BAD_REQUEST;
					goto ExitFunction;
				}
			} else {
				ret_code = HTTP_SERVICE_UNAVAILABLE;
				goto ExitFunction;
			}
		}
		if ((entity_offset + Data_Buf_Size) > parser->msg.entity.length) {
			Data_Buf_Size =
			    parser->msg.entity.length - entity_offset;
		}
		memcpy(Buf,
		       &parser->msg.msg.buf[parser->entity_start_position + entity_offset],
		       Data_Buf_Size);
		entity_offset += Data_Buf_Size;
		if (Instr && Instr->IsVirtualFile) {
			int n = virtualDirCallback.write(Fp, Buf, Data_Buf_Size);
			if (n < 0) {
				ret_code = HTTP_INTERNAL_SERVER_ERROR;
				goto ExitFunction;
			}
		} else {
			size_t n = fwrite(Buf, 1, Data_Buf_Size, Fp);
			if (n != Data_Buf_Size) {
				ret_code = HTTP_INTERNAL_SERVER_ERROR;
				goto ExitFunction;
			}
		}
	} while (parser->position != POS_COMPLETE ||
		 entity_offset != parser->msg.entity.length);
ExitFunction:
	if (Instr && Instr->IsVirtualFile) {
		virtualDirCallback.close(Fp);
	} else {
		fclose(Fp);
	}

	return ret_code;
}
