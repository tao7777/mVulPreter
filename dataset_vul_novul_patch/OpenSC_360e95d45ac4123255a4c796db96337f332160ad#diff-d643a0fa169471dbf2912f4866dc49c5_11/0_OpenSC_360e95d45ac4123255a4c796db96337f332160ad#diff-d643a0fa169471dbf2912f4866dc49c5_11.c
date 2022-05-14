const char * util_acl_to_str(const sc_acl_entry_t *e)
{
	static char line[80], buf[20];
	unsigned int acl;

	if (e == NULL)
		return "N/A";
	line[0] = 0;
	while (e != NULL) {
		acl = e->method;

		switch (acl) {
		case SC_AC_UNKNOWN:
			return "N/A";
		case SC_AC_NEVER:
			return "NEVR";
		case SC_AC_NONE:
			return "NONE";
		case SC_AC_CHV:
			strcpy(buf, "CHV");
			if (e->key_ref != SC_AC_KEY_REF_NONE)
				sprintf(buf + 3, "%d", e->key_ref);
			break;
		case SC_AC_TERM:
			strcpy(buf, "TERM");
			break;
		case SC_AC_PRO:
			strcpy(buf, "PROT");
			break;
		case SC_AC_AUT:
			strcpy(buf, "AUTH");
			if (e->key_ref != SC_AC_KEY_REF_NONE)
				sprintf(buf + 4, "%d", e->key_ref);
			break;
		case SC_AC_SEN:
			strcpy(buf, "Sec.Env. ");
			if (e->key_ref != SC_AC_KEY_REF_NONE)
				sprintf(buf + 3, "#%d", e->key_ref);
			break;
		case SC_AC_SCB:
			strcpy(buf, "Sec.ControlByte ");
			if (e->key_ref != SC_AC_KEY_REF_NONE)
				sprintf(buf + 3, "Ox%X", e->key_ref);
			break;
		case SC_AC_IDA:
			strcpy(buf, "PKCS#15 AuthID ");
			if (e->key_ref != SC_AC_KEY_REF_NONE)
				sprintf(buf + 3, "#%d", e->key_ref);
			break;
		default:
 			strcpy(buf, "????");
 			break;
 		}
		strncat(line, buf, sizeof line);
		strncat(line, " ", sizeof line);
 		e = e->next;
 	}
	line[(sizeof line)-1] = '\0'; /* make sure it's NUL terminated */
 	line[strlen(line)-1] = 0; /* get rid of trailing space */
 	return line;
 }
