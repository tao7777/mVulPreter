ikev2_vid_print(netdissect_options *ndo, u_char tpay,
		const struct isakmp_gen *ext,
		u_int item_len _U_, const u_char *ep _U_,
		uint32_t phase _U_, uint32_t doi _U_,
		uint32_t proto _U_, int depth _U_)
{
	struct isakmp_gen e;
	const u_char *vid;
	int i, len;

	ND_TCHECK(*ext);
	UNALIGNED_MEMCPY(&e, ext, sizeof(e));
	ikev2_pay_print(ndo, NPSTR(tpay), e.critical);
	ND_PRINT((ndo," len=%d vid=", ntohs(e.len) - 4));

	vid = (const u_char *)(ext+1);
	len = ntohs(e.len) - 4;
	ND_TCHECK2(*vid, len);
	for(i=0; i<len; i++) {
		if(ND_ISPRINT(vid[i])) ND_PRINT((ndo, "%c", vid[i]));
 		else ND_PRINT((ndo, "."));
 	}
 	if (2 < ndo->ndo_vflag && 4 < len) {
 		ND_PRINT((ndo," "));
 		if (!rawprint(ndo, (const uint8_t *)(ext + 1), ntohs(e.len) - 4))
 			goto trunc;
	}
	return (const u_char *)ext + ntohs(e.len);
trunc:
	ND_PRINT((ndo," [|%s]", NPSTR(tpay)));
	return NULL;
}
