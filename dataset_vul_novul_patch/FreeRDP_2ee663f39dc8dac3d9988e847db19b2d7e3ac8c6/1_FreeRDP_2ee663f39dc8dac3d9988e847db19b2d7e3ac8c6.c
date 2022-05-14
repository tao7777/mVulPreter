void ntlm_free_message_fields_buffer(NTLM_MESSAGE_FIELDS* fields)
 {
 	if (fields)
 	{
		if (fields->Buffer)
		{
			free(fields->Buffer);
			fields->Len = 0;
			fields->MaxLen = 0;
			fields->Buffer = NULL;
			fields->BufferOffset = 0;
		}
 	}
 }
