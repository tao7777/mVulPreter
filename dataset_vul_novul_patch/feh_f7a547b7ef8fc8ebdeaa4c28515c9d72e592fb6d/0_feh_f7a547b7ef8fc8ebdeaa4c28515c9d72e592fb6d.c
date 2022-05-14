char *enl_ipc_get(const char *msg_data)
 {
 
 	static char *message = NULL;
	static size_t len = 0;
 	char buff[13], *ret_msg = NULL;
 	register unsigned char i;
 	unsigned char blen;

	if (msg_data == IPC_TIMEOUT) {
		return(IPC_TIMEOUT);
	}
	for (i = 0; i < 12; i++) {
		buff[i] = msg_data[i];
	}
	buff[12] = 0;
	blen = strlen(buff);
	if (message != NULL) {
		len += blen;
		message = (char *) erealloc(message, len + 1);
		strcat(message, buff);
	} else {
		len = blen;
		message = (char *) emalloc(len + 1);
		strcpy(message, buff);
	}
	if (blen < 12) {
		ret_msg = message;
		message = NULL;
		D(("Received complete reply:  \"%s\"\n", ret_msg));
	}
	return(ret_msg);
}
