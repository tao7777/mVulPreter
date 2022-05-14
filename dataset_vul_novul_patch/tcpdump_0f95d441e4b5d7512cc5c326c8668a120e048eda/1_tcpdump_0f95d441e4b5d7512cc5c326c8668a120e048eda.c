 ppp_hdlc(netdissect_options *ndo,
          const u_char *p, int length)
 {
	u_char *b, *s, *t, c;
 	int i, proto;
 	const void *se;
 
         if (length <= 0)
                 return;
 
	b = (uint8_t *)malloc(length);
 	if (b == NULL)
 		return;
 
	/*
	 * Unescape all the data into a temporary, private, buffer.
 	 * Do this so that we dont overwrite the original packet
 	 * contents.
 	 */
	for (s = (u_char *)p, t = b, i = length; i > 0; i--) {
 		c = *s++;
 		if (c == 0x7d) {
			if (i > 1) {
				i--;
				c = *s++ ^ 0x20;
			} else
				continue;
 		}
 		*t++ = c;
 	}

	se = ndo->ndo_snapend;
	ndo->ndo_snapend = t;
	length = t - b;

        /* now lets guess about the payload codepoint format */
        if (length < 1)
                goto trunc;
        proto = *b; /* start with a one-octet codepoint guess */

        switch (proto) {
        case PPP_IP:
		ip_print(ndo, b + 1, length - 1);
		goto cleanup;
        case PPP_IPV6:
		ip6_print(ndo, b + 1, length - 1);
		goto cleanup;
        default: /* no luck - try next guess */
		break;
        }

        if (length < 2)
                goto trunc;
        proto = EXTRACT_16BITS(b); /* next guess - load two octets */

        switch (proto) {
        case (PPP_ADDRESS << 8 | PPP_CONTROL): /* looks like a PPP frame */
            if (length < 4)
                goto trunc;
            proto = EXTRACT_16BITS(b+2); /* load the PPP proto-id */
            handle_ppp(ndo, proto, b + 4, length - 4);
            break;
        default: /* last guess - proto must be a PPP proto-id */
            handle_ppp(ndo, proto, b + 2, length - 2);
            break;
        }

cleanup:
	ndo->ndo_snapend = se;
	free(b);
        return;

trunc:
	ndo->ndo_snapend = se;
	free(b);
	ND_PRINT((ndo, "[|ppp]"));
}
