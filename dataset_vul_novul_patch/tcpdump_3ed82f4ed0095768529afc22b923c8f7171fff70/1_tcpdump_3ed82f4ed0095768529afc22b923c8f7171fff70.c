wb_id(netdissect_options *ndo,
      const struct pkt_id *id, u_int len)
{
	int i;
	const char *cp;
	const struct id_off *io;
	char c;
	int nid;

	ND_PRINT((ndo, " wb-id:"));
	if (len < sizeof(*id) || !ND_TTEST(*id))
		return (-1);
	len -= sizeof(*id);

	ND_PRINT((ndo, " %u/%s:%u (max %u/%s:%u) ",
	       EXTRACT_32BITS(&id->pi_ps.slot),
	       ipaddr_string(ndo, &id->pi_ps.page.p_sid),
	       EXTRACT_32BITS(&id->pi_ps.page.p_uid),
	       EXTRACT_32BITS(&id->pi_mslot),
	       ipaddr_string(ndo, &id->pi_mpage.p_sid),
	       EXTRACT_32BITS(&id->pi_mpage.p_uid)));

	nid = EXTRACT_16BITS(&id->pi_ps.nid);
 	len -= sizeof(*io) * nid;
 	io = (struct id_off *)(id + 1);
 	cp = (char *)(io + nid);
	if (!ND_TTEST2(cp, len)) {
 		ND_PRINT((ndo, "\""));
 		fn_print(ndo, (u_char *)cp, (u_char *)cp + len);
 		ND_PRINT((ndo, "\""));
	}

	c = '<';
	for (i = 0; i < nid && ND_TTEST(*io); ++io, ++i) {
		ND_PRINT((ndo, "%c%s:%u",
		    c, ipaddr_string(ndo, &io->id), EXTRACT_32BITS(&io->off)));
		c = ',';
	}
	if (i >= nid) {
		ND_PRINT((ndo, ">"));
		return (0);
	}
	return (-1);
}
