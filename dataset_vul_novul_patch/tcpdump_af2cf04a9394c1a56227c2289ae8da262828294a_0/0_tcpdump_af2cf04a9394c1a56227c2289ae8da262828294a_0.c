bgp_update_print(netdissect_options *ndo,
                 const u_char *dat, int length)
{
	struct bgp bgp;
	const u_char *p;
	int withdrawn_routes_len;
	int len;
	int i;

	ND_TCHECK2(dat[0], BGP_SIZE);
	if (length < BGP_SIZE)
		goto trunc;
	memcpy(&bgp, dat, BGP_SIZE);
	p = dat + BGP_SIZE;	/*XXX*/
	length -= BGP_SIZE;

	/* Unfeasible routes */
	ND_TCHECK2(p[0], 2);
	if (length < 2)
		goto trunc;
	withdrawn_routes_len = EXTRACT_16BITS(p);
	p += 2;
	length -= 2;
	if (withdrawn_routes_len) {
		/*
		 * Without keeping state from the original NLRI message,
		 * it's not possible to tell if this a v4 or v6 route,
		 * so only try to decode it if we're not v6 enabled.
	         */
		ND_TCHECK2(p[0], withdrawn_routes_len);
		if (length < withdrawn_routes_len)
			goto trunc;
		ND_PRINT((ndo, "\n\t  Withdrawn routes: %d bytes", withdrawn_routes_len));
		p += withdrawn_routes_len;
		length -= withdrawn_routes_len;
	}

	ND_TCHECK2(p[0], 2);
	if (length < 2)
		goto trunc;
	len = EXTRACT_16BITS(p);
	p += 2;
	length -= 2;

        if (withdrawn_routes_len == 0 && len == 0 && length == 0) {
            /* No withdrawn routes, no path attributes, no NLRI */
            ND_PRINT((ndo, "\n\t  End-of-Rib Marker (empty NLRI)"));
            return;
        }

	if (len) {
		/* do something more useful!*/
		while (len) {
			int aflags, atype, alenlen, alen;

			ND_TCHECK2(p[0], 2);
			if (len < 2)
			    goto trunc;
			if (length < 2)
			    goto trunc;
			aflags = *p;
			atype = *(p + 1);
			p += 2;
			len -= 2;
			length -= 2;
			alenlen = bgp_attr_lenlen(aflags, p);
			ND_TCHECK2(p[0], alenlen);
			if (len < alenlen)
			    goto trunc;
			if (length < alenlen)
			    goto trunc;
			alen = bgp_attr_len(aflags, p);
			p += alenlen;
			len -= alenlen;
			length -= alenlen;

			ND_PRINT((ndo, "\n\t  %s (%u), length: %u",
                              tok2str(bgp_attr_values, "Unknown Attribute",
					 atype),
                              atype,
                              alen));

			if (aflags) {
				ND_PRINT((ndo, ", Flags [%s%s%s%s",
					aflags & 0x80 ? "O" : "",
					aflags & 0x40 ? "T" : "",
					aflags & 0x20 ? "P" : "",
					aflags & 0x10 ? "E" : ""));
				if (aflags & 0xf)
					ND_PRINT((ndo, "+%x", aflags & 0xf));
				ND_PRINT((ndo, "]: "));
			}
			if (len < alen)
 				goto trunc;
 			if (length < alen)
 				goto trunc;
			if (!bgp_attr_print(ndo, atype, p, alen, 0))
 				goto trunc;
 			p += alen;
 			len -= alen;
			length -= alen;
		}
	}

	if (length) {
		/*
		 * XXX - what if they're using the "Advertisement of
		 * Multiple Paths in BGP" feature:
		 *
		 * https://datatracker.ietf.org/doc/draft-ietf-idr-add-paths/
		 *
		 * http://tools.ietf.org/html/draft-ietf-idr-add-paths-06
		 */
		ND_PRINT((ndo, "\n\t  Updated routes:"));
		while (length) {
			char buf[MAXHOSTNAMELEN + 100];
			i = decode_prefix4(ndo, p, length, buf, sizeof(buf));
			if (i == -1) {
				ND_PRINT((ndo, "\n\t    (illegal prefix length)"));
				break;
			} else if (i == -2)
				goto trunc;
			else if (i == -3)
				goto trunc; /* bytes left, but not enough */
			else {
				ND_PRINT((ndo, "\n\t    %s", buf));
				p += i;
				length -= i;
			}
		}
	}
	return;
trunc:
	ND_PRINT((ndo, "%s", tstr));
}
