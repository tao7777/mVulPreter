void ntlm_populate_message_header(NTLM_MESSAGE_HEADER* header, UINT32 MessageType)
 {
 	CopyMemory(header->Signature, NTLM_SIGNATURE, sizeof(NTLM_SIGNATURE));
 	header->MessageType = MessageType;
 }
