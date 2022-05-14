static char *__filterQuotedShell(const char *arg) {
	r_return_val_if_fail (arg, NULL);
	char *a = malloc (strlen (arg) + 1);
	if (!a) {
		return NULL;
	}
	char *b = a;
	while (*arg) {
 		switch (*arg) {
 		case ' ':
 		case '=':
		case '"':
		case '\\':
 		case '\r':
 		case '\n':
 			break;
		default:
			*b++ = *arg;
			break;
		}
		arg++;
	}
	*b = 0;
	return a;
}
