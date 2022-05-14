 int extract_status_code(char *buffer, size_t size)
 {
 	char *end = buffer + size;
	unsigned long code;
 
 	/* Status-Code extraction */
	while (buffer < end && *buffer != ' ' && *buffer != '\r')
		buffer++;
	buffer++;
	if (buffer + 3 >= end || *buffer == ' ' || buffer[3] != ' ')
		return 0;
	code = strtoul(buffer, &end, 10);
	if (buffer + 3 != end)
		return 0;
 	return code;
 }
