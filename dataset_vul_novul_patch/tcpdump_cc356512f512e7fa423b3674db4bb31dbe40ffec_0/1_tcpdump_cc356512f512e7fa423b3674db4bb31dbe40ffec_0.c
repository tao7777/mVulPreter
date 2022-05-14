wb_print(netdissect_options *ndo,
         register const void *hdr, register u_int len)
{
	register const struct pkt_hdr *ph;

	ph = (const struct pkt_hdr *)hdr;
	if (len < sizeof(*ph) || !ND_TTEST(*ph)) {
		ND_PRINT((ndo, "%s", tstr));
		return;
	}
	len -= sizeof(*ph);

	if (ph->ph_flags)
		ND_PRINT((ndo, "*"));
	switch (ph->ph_type) {

	case PT_KILL:
		ND_PRINT((ndo, " wb-kill"));
		return;

 	case PT_ID:
 		if (wb_id(ndo, (const struct pkt_id *)(ph + 1), len) >= 0)
 			return;
 		break;
 
 	case PT_RREQ:
 		if (wb_rreq(ndo, (const struct pkt_rreq *)(ph + 1), len) >= 0)
 			return;
 		break;
 
 	case PT_RREP:
 		if (wb_rrep(ndo, (const struct pkt_rrep *)(ph + 1), len) >= 0)
 			return;
 		break;
 
 	case PT_DRAWOP:
 		if (wb_drawop(ndo, (const struct pkt_dop *)(ph + 1), len) >= 0)
 			return;
 		break;
 
 	case PT_PREQ:
 		if (wb_preq(ndo, (const struct pkt_preq *)(ph + 1), len) >= 0)
 			return;
 		break;
 
 	case PT_PREP:
 		if (wb_prep(ndo, (const struct pkt_prep *)(ph + 1), len) >= 0)
 			return;
 		break;
 
 	default:
		ND_PRINT((ndo, " wb-%d!", ph->ph_type));
		return;
	}
}
