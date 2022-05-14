ikev1_cr_print(netdissect_options *ndo, u_char tpay _U_,
	       const struct isakmp_gen *ext, u_int item_len,
	       const u_char *ep _U_, uint32_t phase _U_, uint32_t doi0 _U_,
	       uint32_t proto0 _U_, int depth _U_)
{
	const struct ikev1_pl_cert *p;
	struct ikev1_pl_cert cert;
	static const char *certstr[] = {
		"none",	"pkcs7", "pgp", "dns",
		"x509sign", "x509ke", "kerberos", "crl",
		"arl", "spki", "x509attr",
	};

	ND_PRINT((ndo,"%s:", NPSTR(ISAKMP_NPTYPE_CR)));

	p = (const struct ikev1_pl_cert *)ext;
	ND_TCHECK(*p);
	UNALIGNED_MEMCPY(&cert, ext, sizeof(cert));
 	ND_PRINT((ndo," len=%d", item_len - 4));
 	ND_PRINT((ndo," type=%s", STR_OR_ID((cert.encode), certstr)));
 	if (2 < ndo->ndo_vflag && 4 < item_len) {
		/* Print the entire payload in hex */
 		ND_PRINT((ndo," "));
 		if (!rawprint(ndo, (const uint8_t *)(ext + 1), item_len - 4))
 			goto trunc;
	}
	return (const u_char *)ext + item_len;
trunc:
	ND_PRINT((ndo," [|%s]", NPSTR(ISAKMP_NPTYPE_CR)));
	return NULL;
}
