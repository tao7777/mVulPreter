name_len(netdissect_options *ndo,
         const unsigned char *s, const unsigned char *maxbuf)
{
    const unsigned char *s0 = s;
    unsigned char c;

    if (s >= maxbuf)
	return(-1);	/* name goes past the end of the buffer */
    ND_TCHECK2(*s, 1);
    c = *s;
    if ((c & 0xC0) == 0xC0)
	return(2);
    while (*s) {
	if (s >= maxbuf)
 	    return(-1);	/* name goes past the end of the buffer */
 	ND_TCHECK2(*s, 1);
 	s += (*s) + 1;
     }
     return(PTR_DIFF(s, s0) + 1);
 
trunc:
    return(-1);	/* name goes past the end of the buffer */
}
