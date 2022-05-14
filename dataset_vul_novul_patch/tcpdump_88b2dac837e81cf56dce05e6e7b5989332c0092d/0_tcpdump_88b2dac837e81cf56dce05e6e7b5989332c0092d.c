ospf6_decode_v3(netdissect_options *ndo,
                register const struct ospf6hdr *op,
                register const u_char *dataend)
{
	register const rtrid_t *ap;
	register const struct lsr6 *lsrp;
	register const struct lsa6_hdr *lshp;
	register const struct lsa6 *lsap;
	register int i;

	switch (op->ospf6_type) {

 	case OSPF_TYPE_HELLO: {
 		register const struct hello6 *hellop = (const struct hello6 *)((const uint8_t *)op + OSPF6HDR_LEN);
 
		ND_TCHECK_32BITS(&hellop->hello_options);
 		ND_PRINT((ndo, "\n\tOptions [%s]",
 		          bittok2str(ospf6_option_values, "none",
 		          EXTRACT_32BITS(&hellop->hello_options))));

		ND_TCHECK(hellop->hello_deadint);
		ND_PRINT((ndo, "\n\t  Hello Timer %us, Dead Timer %us, Interface-ID %s, Priority %u",
		          EXTRACT_16BITS(&hellop->hello_helloint),
		          EXTRACT_16BITS(&hellop->hello_deadint),
		          ipaddr_string(ndo, &hellop->hello_ifid),
		          hellop->hello_priority));

		ND_TCHECK(hellop->hello_dr);
		if (EXTRACT_32BITS(&hellop->hello_dr) != 0)
			ND_PRINT((ndo, "\n\t  Designated Router %s",
			    ipaddr_string(ndo, &hellop->hello_dr)));
		ND_TCHECK(hellop->hello_bdr);
		if (EXTRACT_32BITS(&hellop->hello_bdr) != 0)
			ND_PRINT((ndo, ", Backup Designated Router %s",
			    ipaddr_string(ndo, &hellop->hello_bdr)));
		if (ndo->ndo_vflag > 1) {
			ND_PRINT((ndo, "\n\t  Neighbor List:"));
			ap = hellop->hello_neighbor;
			while ((const u_char *)ap < dataend) {
				ND_TCHECK(*ap);
				ND_PRINT((ndo, "\n\t    %s", ipaddr_string(ndo, ap)));
				++ap;
			}
		}
		break;	/* HELLO */
	}

	case OSPF_TYPE_DD: {
		register const struct dd6 *ddp = (const struct dd6 *)((const uint8_t *)op + OSPF6HDR_LEN);

		ND_TCHECK(ddp->db_options);
		ND_PRINT((ndo, "\n\tOptions [%s]",
		          bittok2str(ospf6_option_values, "none",
		          EXTRACT_32BITS(&ddp->db_options))));
		ND_TCHECK(ddp->db_flags);
		ND_PRINT((ndo, ", DD Flags [%s]",
		          bittok2str(ospf6_dd_flag_values,"none",ddp->db_flags)));

		ND_TCHECK(ddp->db_seq);
		ND_PRINT((ndo, ", MTU %u, DD-Sequence 0x%08x",
                       EXTRACT_16BITS(&ddp->db_mtu),
                       EXTRACT_32BITS(&ddp->db_seq)));
		if (ndo->ndo_vflag > 1) {
			/* Print all the LS adv's */
			lshp = ddp->db_lshdr;
			while ((const u_char *)lshp < dataend) {
				if (ospf6_print_lshdr(ndo, lshp++, dataend))
					goto trunc;
			}
		}
		break;
	}

	case OSPF_TYPE_LS_REQ:
		if (ndo->ndo_vflag > 1) {
			lsrp = (const struct lsr6 *)((const uint8_t *)op + OSPF6HDR_LEN);
			while ((const u_char *)lsrp < dataend) {
				ND_TCHECK(*lsrp);
				ND_PRINT((ndo, "\n\t  Advertising Router %s",
				          ipaddr_string(ndo, &lsrp->ls_router)));
				ospf6_print_ls_type(ndo, EXTRACT_16BITS(&lsrp->ls_type),
                                                    &lsrp->ls_stateid);
				++lsrp;
			}
		}
		break;

	case OSPF_TYPE_LS_UPDATE:
		if (ndo->ndo_vflag > 1) {
			register const struct lsu6 *lsup = (const struct lsu6 *)((const uint8_t *)op + OSPF6HDR_LEN);

			ND_TCHECK(lsup->lsu_count);
			i = EXTRACT_32BITS(&lsup->lsu_count);
			lsap = lsup->lsu_lsa;
			while ((const u_char *)lsap < dataend && i--) {
				if (ospf6_print_lsa(ndo, lsap, dataend))
					goto trunc;
				lsap = (const struct lsa6 *)((const u_char *)lsap +
				    EXTRACT_16BITS(&lsap->ls_hdr.ls_length));
			}
		}
		break;

	case OSPF_TYPE_LS_ACK:
		if (ndo->ndo_vflag > 1) {
			lshp = (const struct lsa6_hdr *)((const uint8_t *)op + OSPF6HDR_LEN);
			while ((const u_char *)lshp < dataend) {
				if (ospf6_print_lshdr(ndo, lshp++, dataend))
					goto trunc;
			}
		}
		break;

	default:
		break;
	}
	return (0);
trunc:
	return (1);
}
