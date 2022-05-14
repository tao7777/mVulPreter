int ntlm_read_message_header(wStream* s, NTLM_MESSAGE_HEADER* header)
static int ntlm_read_message_header(wStream* s, NTLM_MESSAGE_HEADER* header)
 {
 	if (Stream_GetRemainingLength(s) < 12)
 		return -1;

	Stream_Read(s, header->Signature, 8);
	Stream_Read_UINT32(s, header->MessageType);

	if (strncmp((char*) header->Signature, NTLM_SIGNATURE, 8) != 0)
		return -1;

 	return 1;
 }
