static char *__filterShell(const char *arg) {
	r_return_val_if_fail (arg, NULL);
	char *a = malloc (strlen (arg) + 1);
	if (!a) {
		return NULL;
 	}
 	char *b = a;
 	while (*arg) {
		char ch = *arg;
		switch (ch) {
 		case '@':
 		case '`':
 		case '|':
 		case ';':
		case '=':
 		case '\n':
 			break;
 		default:
			*b++ = ch;
 			break;
 		}
 		arg++;
	}
	*b = 0;
	return a;
}
