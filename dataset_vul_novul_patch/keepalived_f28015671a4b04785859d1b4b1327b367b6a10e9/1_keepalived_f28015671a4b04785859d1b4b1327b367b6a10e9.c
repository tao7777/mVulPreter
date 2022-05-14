 int extract_status_code(char *buffer, size_t size)
 {
	char *buf_code;
	char *begin;
 	char *end = buffer + size;
	size_t inc = 0;
	int code;
	/* Allocate the room */
	buf_code = (char *)MALLOC(10);
 
 	/* Status-Code extraction */
	while (buffer < end && *buffer++ != ' ') ;
	begin = buffer;
	while (buffer < end && *buffer++ != ' ')
		inc++;
	strncat(buf_code, begin, inc);
	code = atoi(buf_code);
	FREE(buf_code);
 	return code;
 }
