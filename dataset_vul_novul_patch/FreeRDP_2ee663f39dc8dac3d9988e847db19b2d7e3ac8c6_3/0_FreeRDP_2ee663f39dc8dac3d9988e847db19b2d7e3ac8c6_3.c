int ntlm_read_message_fields(wStream* s, NTLM_MESSAGE_FIELDS* fields)
static int ntlm_read_message_fields(wStream* s, NTLM_MESSAGE_FIELDS* fields)
 {
 	if (Stream_GetRemainingLength(s) < 8)
 		return -1;

	Stream_Read_UINT16(s, fields->Len); /* Len (2 bytes) */
	Stream_Read_UINT16(s, fields->MaxLen); /* MaxLen (2 bytes) */
	Stream_Read_UINT32(s, fields->BufferOffset); /* BufferOffset (4 bytes) */
 	return 1;
 }
