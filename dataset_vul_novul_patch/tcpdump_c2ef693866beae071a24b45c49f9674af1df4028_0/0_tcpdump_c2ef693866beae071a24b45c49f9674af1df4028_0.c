ikev1_nonce_print(netdissect_options *ndo, u_char tpay _U_,
		  const struct isakmp_gen *ext,
		  u_int item_len _U_,
		  const u_char *ep,
		  uint32_t phase _U_, uint32_t doi _U_,
		  uint32_t proto _U_, int depth _U_)
{
	struct isakmp_gen e;

	ND_PRINT((ndo,"%s:", NPSTR(ISAKMP_NPTYPE_NONCE)));
 
 	ND_TCHECK(*ext);
 	UNALIGNED_MEMCPY(&e, ext, sizeof(e));
	/*
	 * Our caller has ensured that the length is >= 4.
	 */
	ND_PRINT((ndo," n len=%u", ntohs(e.len) - 4));
	if (ntohs(e.len) > 4) {
		if (ndo->ndo_vflag > 2) {
			ND_PRINT((ndo, " "));
			if (!rawprint(ndo, (const uint8_t *)(ext + 1), ntohs(e.len) - 4))
				goto trunc;
		} else if (ndo->ndo_vflag > 1) {
			ND_PRINT((ndo, " "));
			if (!ike_show_somedata(ndo, (const u_char *)(ext + 1), ep))
				goto trunc;
		}
 	}
 	return (const u_char *)ext + ntohs(e.len);
 trunc:
	ND_PRINT((ndo," [|%s]", NPSTR(ISAKMP_NPTYPE_NONCE)));
	return NULL;
}
