telnet_parse(netdissect_options *ndo, const u_char *sp, u_int length, int print)
{
	int i, x;
	u_int c;
	const u_char *osp, *p;
#define FETCH(c, sp, length) \
	do { \
		if (length < 1) \
			goto pktend; \
		ND_TCHECK(*sp); \
		c = *sp++; \
		length--; \
	} while (0)

	osp = sp;

	FETCH(c, sp, length);
	if (c != IAC)
		goto pktend;
	FETCH(c, sp, length);
	if (c == IAC) {		/* <IAC><IAC>! */
		if (print)
			ND_PRINT((ndo, "IAC IAC"));
		goto done;
	}

	i = c - TELCMD_FIRST;
	if (i < 0 || i > IAC - TELCMD_FIRST)
		goto pktend;

	switch (c) {
	case DONT:
	case DO:
	case WONT:
	case WILL:
	case SB:
		/* DONT/DO/WONT/WILL x */
		FETCH(x, sp, length);
		if (x >= 0 && x < NTELOPTS) {
			if (print)
				ND_PRINT((ndo, "%s %s", telcmds[i], telopts[x]));
		} else {
			if (print)
				ND_PRINT((ndo, "%s %#x", telcmds[i], x));
		}
		if (c != SB)
			break;
		/* IAC SB .... IAC SE */
		p = sp;
		while (length > (u_int)(p + 1 - sp)) {
			ND_TCHECK2(*p, 2);
			if (p[0] == IAC && p[1] == SE)
 				break;
 			p++;
 		}
		ND_TCHECK(*p);
 		if (*p != IAC)
 			goto pktend;
 
		switch (x) {
		case TELOPT_AUTHENTICATION:
			if (p <= sp)
				break;
			FETCH(c, sp, length);
			if (print)
				ND_PRINT((ndo, " %s", STR_OR_ID(c, authcmd)));
			if (p <= sp)
				break;
			FETCH(c, sp, length);
			if (print)
				ND_PRINT((ndo, " %s", STR_OR_ID(c, authtype)));
			break;
		case TELOPT_ENCRYPT:
			if (p <= sp)
				break;
			FETCH(c, sp, length);
			if (print)
				ND_PRINT((ndo, " %s", STR_OR_ID(c, enccmd)));
			if (p <= sp)
				break;
			FETCH(c, sp, length);
			if (print)
				ND_PRINT((ndo, " %s", STR_OR_ID(c, enctype)));
			break;
		default:
			if (p <= sp)
				break;
			FETCH(c, sp, length);
			if (print)
				ND_PRINT((ndo, " %s", STR_OR_ID(c, cmds)));
			break;
		}
		while (p > sp) {
			FETCH(x, sp, length);
			if (print)
				ND_PRINT((ndo, " %#x", x));
		}
		/* terminating IAC SE */
		if (print)
			ND_PRINT((ndo, " SE"));
		sp += 2;
		break;
	default:
		if (print)
			ND_PRINT((ndo, "%s", telcmds[i]));
		goto done;
	}

done:
	return sp - osp;

trunc:
	ND_PRINT((ndo, "%s", tstr));
pktend:
	return -1;
#undef FETCH
}
