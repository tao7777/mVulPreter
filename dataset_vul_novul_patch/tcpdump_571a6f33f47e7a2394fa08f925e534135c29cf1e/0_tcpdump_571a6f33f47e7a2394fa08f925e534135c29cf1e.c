isis_print_id(const uint8_t *cp, int id_len)
{
     int i;
     static char id[sizeof("xxxx.xxxx.xxxx.yy-zz")];
     char *pos = id;
    int sysid_len;
 
    sysid_len = SYSTEM_ID_LEN;
    if (sysid_len > id_len)
        sysid_len = id_len;
    for (i = 1; i <= sysid_len; i++) {
         snprintf(pos, sizeof(id) - (pos - id), "%02x", *cp++);
 	pos += strlen(pos);
 	if (i == 2 || i == 4)
	    *pos++ = '.';
	}
    if (id_len >= NODE_ID_LEN) {
        snprintf(pos, sizeof(id) - (pos - id), ".%02x", *cp++);
	pos += strlen(pos);
    }
    if (id_len == LSP_ID_LEN)
        snprintf(pos, sizeof(id) - (pos - id), "-%02x", *cp);
    return (id);
}
