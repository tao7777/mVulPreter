x11_open_helper(Buffer *b)
{
	u_char *ucp;
	u_int proto_len, data_len;

 	u_char *ucp;
 	u_int proto_len, data_len;
 
 	/* Check if the fixed size part of the packet is in buffer. */
 	if (buffer_len(b) < 12)
 		return 0;
		debug2("Initial X11 packet contains bad byte order byte: 0x%x",
		    ucp[0]);
		return -1;
	}
