ikev2_gen_print(netdissect_options *ndo, u_char tpay,
		const struct isakmp_gen *ext)
{
	struct isakmp_gen e;

	ND_TCHECK(*ext);
	UNALIGNED_MEMCPY(&e, ext, sizeof(e));
	ikev2_pay_print(ndo, NPSTR(tpay), e.critical);
 
 	ND_PRINT((ndo," len=%d", ntohs(e.len) - 4));
 	if (2 < ndo->ndo_vflag && 4 < ntohs(e.len)) {
		/* Print the entire payload in hex */
 		ND_PRINT((ndo," "));
 		if (!rawprint(ndo, (const uint8_t *)(ext + 1), ntohs(e.len) - 4))
 			goto trunc;
	}
	return (const u_char *)ext + ntohs(e.len);
trunc:
	ND_PRINT((ndo," [|%s]", NPSTR(tpay)));
	return NULL;
}
