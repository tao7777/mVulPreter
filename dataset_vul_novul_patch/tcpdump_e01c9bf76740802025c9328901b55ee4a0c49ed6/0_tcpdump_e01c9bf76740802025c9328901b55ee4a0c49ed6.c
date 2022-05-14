ospf6_print_lshdr(netdissect_options *ndo,
                  register const struct lsa6_hdr *lshp, const u_char *dataend)
 {
 	if ((const u_char *)(lshp + 1) > dataend)
 		goto trunc;
	ND_TCHECK(lshp->ls_length);	/* last field of struct lsa6_hdr */
 
 	ND_PRINT((ndo, "\n\t  Advertising Router %s, seq 0x%08x, age %us, length %u",
                ipaddr_string(ndo, &lshp->ls_router),
               EXTRACT_32BITS(&lshp->ls_seq),
               EXTRACT_16BITS(&lshp->ls_age),
               EXTRACT_16BITS(&lshp->ls_length)-(u_int)sizeof(struct lsa6_hdr)));

	ospf6_print_ls_type(ndo, EXTRACT_16BITS(&lshp->ls_type), &lshp->ls_stateid);

	return (0);
trunc:
	return (1);
}
