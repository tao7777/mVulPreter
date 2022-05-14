void ntlm_write_message_header(wStream* s, NTLM_MESSAGE_HEADER* header)
 {
 	Stream_Write(s, header->Signature, sizeof(NTLM_SIGNATURE));
 	Stream_Write_UINT32(s, header->MessageType);
 }
