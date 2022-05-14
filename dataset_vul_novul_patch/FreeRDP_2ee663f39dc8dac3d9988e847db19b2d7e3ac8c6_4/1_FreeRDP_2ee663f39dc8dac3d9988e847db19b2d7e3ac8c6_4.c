int ntlm_read_message_fields_buffer(wStream* s, NTLM_MESSAGE_FIELDS* fields)
 {
 	if (fields->Len > 0)
 	{
		if ((fields->BufferOffset + fields->Len) > Stream_Length(s))
 			return -1;
 
 		fields->Buffer = (PBYTE) malloc(fields->Len);

		if (!fields->Buffer)
			return -1;

		Stream_SetPosition(s, fields->BufferOffset);
		Stream_Read(s, fields->Buffer, fields->Len);
	}

 	return 1;
 }
