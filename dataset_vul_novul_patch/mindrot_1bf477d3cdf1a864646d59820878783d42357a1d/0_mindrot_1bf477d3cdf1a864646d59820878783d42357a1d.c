x11_open_helper(Buffer *b)
{
	u_char *ucp;
	u_int proto_len, data_len;

 	u_char *ucp;
 	u_int proto_len, data_len;
 
	/* Is this being called after the refusal deadline? */
	if (x11_refuse_time != 0 && (u_int)monotime() >= x11_refuse_time) {
		verbose("Rejected X11 connection after ForwardX11Timeout "
		    "expired");
		return -1;
	}

 	/* Check if the fixed size part of the packet is in buffer. */
 	if (buffer_len(b) < 12)
 		return 0;
		debug2("Initial X11 packet contains bad byte order byte: 0x%x",
		    ucp[0]);
		return -1;
	}
