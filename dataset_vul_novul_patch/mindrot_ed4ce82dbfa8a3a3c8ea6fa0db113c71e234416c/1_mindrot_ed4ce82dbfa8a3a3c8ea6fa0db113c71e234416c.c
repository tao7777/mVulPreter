client_x11_display_valid(const char *display)
 {
 	size_t i, dlen;
 
 	dlen = strlen(display);
 	for (i = 0; i < dlen; i++) {
 		if (!isalnum((u_char)display[i]) &&
		}
	}
