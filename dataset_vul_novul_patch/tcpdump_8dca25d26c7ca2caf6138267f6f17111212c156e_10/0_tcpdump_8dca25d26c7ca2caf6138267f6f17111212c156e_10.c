ikev2_p_print(netdissect_options *ndo, u_char tpay _U_, int pcount _U_,
	      const struct isakmp_gen *ext, u_int oprop_length,
	      const u_char *ep, int depth)
{
	const struct ikev2_p *p;
	struct ikev2_p prop;
	u_int prop_length;
	const u_char *cp;
	int i;
	int tcount;
	u_char np;
	struct isakmp_gen e;
	u_int item_len;

	p = (const struct ikev2_p *)ext;
	ND_TCHECK(*p);
	UNALIGNED_MEMCPY(&prop, ext, sizeof(prop));

	ikev2_pay_print(ndo, NPSTR(ISAKMP_NPTYPE_P), prop.h.critical);

	/*
	 * ikev2_sa_print() guarantees that this is >= 4.
	 */
	prop_length = oprop_length - 4;
	ND_PRINT((ndo," #%u protoid=%s transform=%d len=%u",
		  prop.p_no,  PROTOIDSTR(prop.prot_id),
		  prop.num_t, oprop_length));
	cp = (const u_char *)(p + 1);

	if (prop.spi_size) {
		if (prop_length < prop.spi_size)
			goto toolong;
		ND_PRINT((ndo," spi="));
		if (!rawprint(ndo, (const uint8_t *)cp, prop.spi_size))
			goto trunc;
		cp += prop.spi_size;
		prop_length -= prop.spi_size;
	}

	/*
	 * Print the transforms.
	 */
	tcount = 0;
	for (np = ISAKMP_NPTYPE_T; np != 0; np = e.np) {
		tcount++;
		ext = (const struct isakmp_gen *)cp;
 		if (prop_length < sizeof(*ext))
 			goto toolong;
 		ND_TCHECK(*ext);
 		UNALIGNED_MEMCPY(&e, ext, sizeof(e));
 
 		/*
		 * Since we can't have a payload length of less than 4 bytes,
		 * we need to bail out here if the generic header is nonsensical
		 * or truncated, otherwise we could loop forever processing
		 * zero-length items or otherwise misdissect the packet.
		 */
		item_len = ntohs(e.len);
		if (item_len <= 4)
			goto trunc;

		if (prop_length < item_len)
			goto toolong;
		ND_TCHECK2(*cp, item_len);

		depth++;
		ND_PRINT((ndo,"\n"));
		for (i = 0; i < depth; i++)
			ND_PRINT((ndo,"    "));
		ND_PRINT((ndo,"("));
		if (np == ISAKMP_NPTYPE_T) {
			cp = ikev2_t_print(ndo, tcount, ext, item_len, ep);
			if (cp == NULL) {
				/* error, already reported */
				return NULL;
			}
		} else {
			ND_PRINT((ndo, "%s", NPSTR(np)));
			cp += item_len;
		}
		ND_PRINT((ndo,")"));
		depth--;
		prop_length -= item_len;
	}
	return cp;
toolong:
	/*
	 * Skip the rest of the proposal.
	 */
	cp += prop_length;
	ND_PRINT((ndo," [|%s]", NPSTR(ISAKMP_NPTYPE_P)));
	return cp;
trunc:
	ND_PRINT((ndo," [|%s]", NPSTR(ISAKMP_NPTYPE_P)));
	return NULL;
}
