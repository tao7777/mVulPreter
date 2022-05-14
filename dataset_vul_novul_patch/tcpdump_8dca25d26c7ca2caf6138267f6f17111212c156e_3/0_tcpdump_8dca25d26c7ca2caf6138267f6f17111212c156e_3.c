ikev1_sig_print(netdissect_options *ndo, u_char tpay _U_,
		const struct isakmp_gen *ext, u_int item_len _U_,
		const u_char *ep _U_, uint32_t phase _U_, uint32_t doi _U_,
		uint32_t proto _U_, int depth _U_)
{
	struct isakmp_gen e;

	ND_PRINT((ndo,"%s:", NPSTR(ISAKMP_NPTYPE_SIG)));

	ND_TCHECK(*ext);
 	UNALIGNED_MEMCPY(&e, ext, sizeof(e));
 	ND_PRINT((ndo," len=%d", ntohs(e.len) - 4));
 	if (2 < ndo->ndo_vflag && 4 < ntohs(e.len)) {
		/* Print the entire payload in hex */
 		ND_PRINT((ndo," "));
 		if (!rawprint(ndo, (const uint8_t *)(ext + 1), ntohs(e.len) - 4))
 			goto trunc;
	}
	return (const u_char *)ext + ntohs(e.len);
trunc:
	ND_PRINT((ndo," [|%s]", NPSTR(ISAKMP_NPTYPE_SIG)));
	return NULL;
}
