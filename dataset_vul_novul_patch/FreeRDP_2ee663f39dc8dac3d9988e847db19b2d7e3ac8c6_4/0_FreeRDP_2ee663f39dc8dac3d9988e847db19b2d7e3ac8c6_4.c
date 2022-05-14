int ntlm_read_message_fields_buffer(wStream* s, NTLM_MESSAGE_FIELDS* fields)
static int ntlm_read_message_fields_buffer(wStream* s, NTLM_MESSAGE_FIELDS* fields)
 {
 	if (fields->Len > 0)
 	{
		const UINT64 offset = (UINT64)fields->BufferOffset + (UINT64)fields->Len;

		if (offset > Stream_Length(s))
 			return -1;
 
 		fields->Buffer = (PBYTE) malloc(fields->Len);

		if (!fields->Buffer)
			return -1;

		Stream_SetPosition(s, fields->BufferOffset);
		Stream_Read(s, fields->Buffer, fields->Len);
	}

 	return 1;
 }
