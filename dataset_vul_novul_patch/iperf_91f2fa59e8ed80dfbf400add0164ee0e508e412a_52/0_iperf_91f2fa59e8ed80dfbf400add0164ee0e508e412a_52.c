iperf_json_printf(const char *format, ...)
{
    cJSON* o;
    va_list argp;
    const char *cp;
    char name[100];
    char* np;
    cJSON* j;

    o = cJSON_CreateObject();
    if (o == NULL)
        return NULL;
    va_start(argp, format);
    np = name;
    for (cp = format; *cp != '\0'; ++cp) {
	switch (*cp) {
	    case ' ':
	    break;
	    case ':':
	    *np = '\0';
	    break;
	    case '%':
	    ++cp;
	    switch (*cp) {
		case 'b':
 		j = cJSON_CreateBool(va_arg(argp, int));
 		break;
 		case 'd':
		j = cJSON_CreateNumber(va_arg(argp, int64_t));
 		break;
 		case 'f':
		j = cJSON_CreateNumber(va_arg(argp, double));
 		break;
 		case 's':
 		j = cJSON_CreateString(va_arg(argp, char *));
		break;
		default:
		return NULL;
	    }
	    if (j == NULL)
		return NULL;
	    cJSON_AddItemToObject(o, name, j);
	    np = name;
	    break;
	    default:
	    *np++ = *cp;
	    break;
	}
    }
    va_end(argp);
    return o;
}
