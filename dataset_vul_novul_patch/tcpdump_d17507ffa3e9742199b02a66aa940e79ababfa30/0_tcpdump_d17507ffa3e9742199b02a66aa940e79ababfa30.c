parse_field(netdissect_options *ndo, const char **pptr, int *len)
parse_field(netdissect_options *ndo, const char **pptr, int *len, int *truncated)
 {
     const char *s;
 
    /* Start of string */
     s = *pptr;
    /* Scan for the NUL terminator */
    for (;;) {
	if (*len == 0) {
	    /* Ran out of packet data without finding it */
	    return NULL;
	}
	if (!ND_TTEST(**pptr)) {
	    /* Ran out of captured data without finding it */
	    *truncated = 1;
	    return NULL;
	}
	if (**pptr == '\0') {
	    /* Found it */
	    break;
	}
	/* Keep scanning */
 	(*pptr)++;
 	(*len)--;
     }
    /* Skip the NUL terminator */
     (*pptr)++;
     (*len)--;
     return s;
 }
