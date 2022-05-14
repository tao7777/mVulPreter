 static inline int process_numeric_entity(const char **buf, unsigned *code_point)
 {
	long code_l;
	int hexadecimal = (**buf == 'x' || **buf == 'X'); /* TODO: XML apparently disallows "X" */
	char *endptr;
 
 	if (hexadecimal && (**buf != '\0'))
 		(*buf)++;
 	/* strtol allows whitespace and other stuff in the beginning
 		* we're not interested */
 	if ((hexadecimal && !isxdigit(**buf)) ||
			(!hexadecimal && !isdigit(**buf))) {
		return FAILURE;
	}

	code_l = strtol(*buf, &endptr, hexadecimal ? 16 : 10);
	/* we're guaranteed there were valid digits, so *endptr > buf */
	*buf = endptr;

	if (**buf != ';')
		return FAILURE;

	/* many more are invalid, but that depends on whether it's HTML
	 * (and which version) or XML. */
	if (code_l > 0x10FFFFL)
		return FAILURE;

	if (code_point != NULL)
		*code_point = (unsigned)code_l;

	return SUCCESS;
}
