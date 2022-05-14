ikev2_ke_print(netdissect_options *ndo, u_char tpay,
		const struct isakmp_gen *ext,
		u_int item_len _U_, const u_char *ep _U_,
		uint32_t phase _U_, uint32_t doi _U_,
		uint32_t proto _U_, int depth _U_)
{
	struct ikev2_ke ke;
 	const struct ikev2_ke *k;
 
 	k = (const struct ikev2_ke *)ext;
	ND_TCHECK(*k);
 	UNALIGNED_MEMCPY(&ke, ext, sizeof(ke));
 	ikev2_pay_print(ndo, NPSTR(tpay), ke.h.critical);
 
	ND_PRINT((ndo," len=%u group=%s", ntohs(ke.h.len) - 8,
		  STR_OR_ID(ntohs(ke.ke_group), dh_p_map)));

	if (2 < ndo->ndo_vflag && 8 < ntohs(ke.h.len)) {
		ND_PRINT((ndo," "));
		if (!rawprint(ndo, (const uint8_t *)(k + 1), ntohs(ke.h.len) - 8))
			goto trunc;
	}
	return (const u_char *)ext + ntohs(ke.h.len);
trunc:
	ND_PRINT((ndo," [|%s]", NPSTR(tpay)));
	return NULL;
}
