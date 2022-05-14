wb_prep(netdissect_options *ndo,
        const struct pkt_prep *prep, u_int len)
{
	int n;
	const struct pgstate *ps;
 	const u_char *ep = ndo->ndo_snapend;
 
 	ND_PRINT((ndo, " wb-prep:"));
	if (len < sizeof(*prep) || !ND_TTEST(*prep))
 		return (-1);
 	n = EXTRACT_32BITS(&prep->pp_n);
 	ps = (const struct pgstate *)(prep + 1);
 	while (--n >= 0 && ND_TTEST(*ps)) {
		const struct id_off *io, *ie;
		char c = '<';

		ND_PRINT((ndo, " %u/%s:%u",
		    EXTRACT_32BITS(&ps->slot),
		    ipaddr_string(ndo, &ps->page.p_sid),
		    EXTRACT_32BITS(&ps->page.p_uid)));
		io = (const struct id_off *)(ps + 1);
		for (ie = io + ps->nid; io < ie && ND_TTEST(*io); ++io) {
			ND_PRINT((ndo, "%c%s:%u", c, ipaddr_string(ndo, &io->id),
			    EXTRACT_32BITS(&io->off)));
			c = ',';
		}
		ND_PRINT((ndo, ">"));
		ps = (const struct pgstate *)io;
	}
	return ((const u_char *)ps <= ep? 0 : -1);
}
