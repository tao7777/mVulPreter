void ntlm_write_message_fields_buffer(wStream* s, NTLM_MESSAGE_FIELDS* fields)
static void ntlm_write_message_fields_buffer(wStream* s, NTLM_MESSAGE_FIELDS* fields)
 {
 	if (fields->Len > 0)
 	{
		Stream_SetPosition(s, fields->BufferOffset);
		Stream_Write(s, fields->Buffer, fields->Len);
 	}
 }
