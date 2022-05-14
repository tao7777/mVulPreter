decnet_print(netdissect_options *ndo,
             register const u_char *ap, register u_int length,
             register u_int caplen)
{
	register const union routehdr *rhp;
	register int mflags;
	int dst, src, hops;
	u_int nsplen, pktlen;
	const u_char *nspp;

	if (length < sizeof(struct shorthdr)) {
		ND_PRINT((ndo, "%s", tstr));
		return;
	}

	ND_TCHECK2(*ap, sizeof(short));
	pktlen = EXTRACT_LE_16BITS(ap);
	if (pktlen < sizeof(struct shorthdr)) {
		ND_PRINT((ndo, "%s", tstr));
		return;
	}
	if (pktlen > length) {
		ND_PRINT((ndo, "%s", tstr));
		return;
	}
	length = pktlen;

	rhp = (const union routehdr *)&(ap[sizeof(short)]);
	ND_TCHECK(rhp->rh_short.sh_flags);
	mflags = EXTRACT_LE_8BITS(rhp->rh_short.sh_flags);

	if (mflags & RMF_PAD) {
	    /* pad bytes of some sort in front of message */
	    u_int padlen = mflags & RMF_PADMASK;
	    if (ndo->ndo_vflag)
		ND_PRINT((ndo, "[pad:%d] ", padlen));
	    if (length < padlen + 2) {
		ND_PRINT((ndo, "%s", tstr));
		return;
	    }
	    ND_TCHECK2(ap[sizeof(short)], padlen);
	    ap += padlen;
 	    length -= padlen;
 	    caplen -= padlen;
 	    rhp = (const union routehdr *)&(ap[sizeof(short)]);
	    ND_TCHECK(rhp->rh_short.sh_flags);
 	    mflags = EXTRACT_LE_8BITS(rhp->rh_short.sh_flags);
 	}
 
	if (mflags & RMF_FVER) {
		ND_PRINT((ndo, "future-version-decnet"));
		ND_DEFAULTPRINT(ap, min(length, caplen));
		return;
	}

	/* is it a control message? */
	if (mflags & RMF_CTLMSG) {
		if (!print_decnet_ctlmsg(ndo, rhp, length, caplen))
			goto trunc;
		return;
	}

	switch (mflags & RMF_MASK) {
	case RMF_LONG:
	    if (length < sizeof(struct longhdr)) {
		ND_PRINT((ndo, "%s", tstr));
		return;
	    }
	    ND_TCHECK(rhp->rh_long);
	    dst =
		EXTRACT_LE_16BITS(rhp->rh_long.lg_dst.dne_remote.dne_nodeaddr);
	    src =
		EXTRACT_LE_16BITS(rhp->rh_long.lg_src.dne_remote.dne_nodeaddr);
	    hops = EXTRACT_LE_8BITS(rhp->rh_long.lg_visits);
	    nspp = &(ap[sizeof(short) + sizeof(struct longhdr)]);
	    nsplen = length - sizeof(struct longhdr);
	    break;
	case RMF_SHORT:
	    ND_TCHECK(rhp->rh_short);
	    dst = EXTRACT_LE_16BITS(rhp->rh_short.sh_dst);
	    src = EXTRACT_LE_16BITS(rhp->rh_short.sh_src);
	    hops = (EXTRACT_LE_8BITS(rhp->rh_short.sh_visits) & VIS_MASK)+1;
	    nspp = &(ap[sizeof(short) + sizeof(struct shorthdr)]);
	    nsplen = length - sizeof(struct shorthdr);
	    break;
	default:
	    ND_PRINT((ndo, "unknown message flags under mask"));
	    ND_DEFAULTPRINT((const u_char *)ap, min(length, caplen));
	    return;
	}

	ND_PRINT((ndo, "%s > %s %d ",
			dnaddr_string(ndo, src), dnaddr_string(ndo, dst), pktlen));
	if (ndo->ndo_vflag) {
	    if (mflags & RMF_RQR)
		ND_PRINT((ndo, "RQR "));
	    if (mflags & RMF_RTS)
		ND_PRINT((ndo, "RTS "));
	    if (mflags & RMF_IE)
		ND_PRINT((ndo, "IE "));
	    ND_PRINT((ndo, "%d hops ", hops));
	}

	if (!print_nsp(ndo, nspp, nsplen))
		goto trunc;
	return;

trunc:
	ND_PRINT((ndo, "%s", tstr));
	return;
}
