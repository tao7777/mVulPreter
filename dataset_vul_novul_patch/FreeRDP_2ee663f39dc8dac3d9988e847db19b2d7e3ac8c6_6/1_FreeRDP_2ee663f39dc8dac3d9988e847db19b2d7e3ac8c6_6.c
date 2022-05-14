void ntlm_write_message_fields(wStream* s, NTLM_MESSAGE_FIELDS* fields)
 {
 	if (fields->MaxLen < 1)
 		fields->MaxLen = fields->Len;

	Stream_Write_UINT16(s, fields->Len); /* Len (2 bytes) */
	Stream_Write_UINT16(s, fields->MaxLen); /* MaxLen (2 bytes) */
 	Stream_Write_UINT32(s, fields->BufferOffset); /* BufferOffset (4 bytes) */
 }
