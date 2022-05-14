parserep(netdissect_options *ndo,
         register const struct sunrpc_msg *rp, register u_int length)
{
	register const uint32_t *dp;
	u_int len;
	enum sunrpc_accept_stat astat;

	/*
	 * Portability note:
	 * Here we find the address of the ar_verf credentials.
	 * Originally, this calculation was
	 *	dp = (uint32_t *)&rp->rm_reply.rp_acpt.ar_verf
	 * On the wire, the rp_acpt field starts immediately after
	 * the (32 bit) rp_stat field.  However, rp_acpt (which is a
	 * "struct accepted_reply") contains a "struct opaque_auth",
	 * whose internal representation contains a pointer, so on a
	 * 64-bit machine the compiler inserts 32 bits of padding
	 * before rp->rm_reply.rp_acpt.ar_verf.  So, we cannot use
	 * the internal representation to parse the on-the-wire
	 * representation.  Instead, we skip past the rp_stat field,
	 * which is an "enum" and so occupies one 32-bit word.
	 */
	dp = ((const uint32_t *)&rp->rm_reply) + 1;
	ND_TCHECK(dp[1]);
	len = EXTRACT_32BITS(&dp[1]);
	if (len >= length)
		return (NULL);
	/*
 	 * skip past the ar_verf credentials.
 	 */
 	dp += (len + (2*sizeof(uint32_t) + 3)) / sizeof(uint32_t);
	ND_TCHECK2(dp[0], 0);
 
 	/*
 	 * now we can check the ar_stat field
 	 */
 	astat = (enum sunrpc_accept_stat) EXTRACT_32BITS(dp);
 	if (astat != SUNRPC_SUCCESS) {
 		ND_PRINT((ndo, " %s", tok2str(sunrpc_str, "ar_stat %d", astat)));
		nfserr = 1;		/* suppress trunc string */
		return (NULL);
	}
	/* successful return */
	ND_TCHECK2(*dp, sizeof(astat));
	return ((const uint32_t *) (sizeof(astat) + ((const char *)dp)));
trunc:
	return (0);
}
