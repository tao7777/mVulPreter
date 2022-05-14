bootp_print(netdissect_options *ndo,
	    register const u_char *cp, u_int length)
{
	register const struct bootp *bp;
	static const u_char vm_cmu[4] = VM_CMU;
	static const u_char vm_rfc1048[4] = VM_RFC1048;

	bp = (const struct bootp *)cp;
	ND_TCHECK(bp->bp_op);

	ND_PRINT((ndo, "BOOTP/DHCP, %s",
		  tok2str(bootp_op_values, "unknown (0x%02x)", bp->bp_op)));

	ND_TCHECK(bp->bp_hlen);
	if (bp->bp_htype == 1 && bp->bp_hlen == 6 && bp->bp_op == BOOTPREQUEST) {
		ND_TCHECK2(bp->bp_chaddr[0], 6);
		ND_PRINT((ndo, " from %s", etheraddr_string(ndo, bp->bp_chaddr)));
	}

	ND_PRINT((ndo, ", length %u", length));

	if (!ndo->ndo_vflag)
		return;

	ND_TCHECK(bp->bp_secs);

	/* The usual hardware address type is 1 (10Mb Ethernet) */
	if (bp->bp_htype != 1)
		ND_PRINT((ndo, ", htype %d", bp->bp_htype));

	/* The usual length for 10Mb Ethernet address is 6 bytes */
	if (bp->bp_htype != 1 || bp->bp_hlen != 6)
		ND_PRINT((ndo, ", hlen %d", bp->bp_hlen));

	/* Only print interesting fields */
	if (bp->bp_hops)
		ND_PRINT((ndo, ", hops %d", bp->bp_hops));
	if (EXTRACT_32BITS(&bp->bp_xid))
		ND_PRINT((ndo, ", xid 0x%x", EXTRACT_32BITS(&bp->bp_xid)));
 	if (EXTRACT_16BITS(&bp->bp_secs))
 		ND_PRINT((ndo, ", secs %d", EXTRACT_16BITS(&bp->bp_secs)));
 
 	ND_PRINT((ndo, ", Flags [%s]",
 		  bittok2str(bootp_flag_values, "none", EXTRACT_16BITS(&bp->bp_flags))));
 	if (ndo->ndo_vflag > 1)
		ND_PRINT((ndo, " (0x%04x)", EXTRACT_16BITS(&bp->bp_flags)));

	/* Client's ip address */
	ND_TCHECK(bp->bp_ciaddr);
	if (EXTRACT_32BITS(&bp->bp_ciaddr.s_addr))
		ND_PRINT((ndo, "\n\t  Client-IP %s", ipaddr_string(ndo, &bp->bp_ciaddr)));

	/* 'your' ip address (bootp client) */
	ND_TCHECK(bp->bp_yiaddr);
	if (EXTRACT_32BITS(&bp->bp_yiaddr.s_addr))
		ND_PRINT((ndo, "\n\t  Your-IP %s", ipaddr_string(ndo, &bp->bp_yiaddr)));

	/* Server's ip address */
	ND_TCHECK(bp->bp_siaddr);
	if (EXTRACT_32BITS(&bp->bp_siaddr.s_addr))
		ND_PRINT((ndo, "\n\t  Server-IP %s", ipaddr_string(ndo, &bp->bp_siaddr)));

	/* Gateway's ip address */
	ND_TCHECK(bp->bp_giaddr);
	if (EXTRACT_32BITS(&bp->bp_giaddr.s_addr))
		ND_PRINT((ndo, "\n\t  Gateway-IP %s", ipaddr_string(ndo, &bp->bp_giaddr)));

	/* Client's Ethernet address */
	if (bp->bp_htype == 1 && bp->bp_hlen == 6) {
		ND_TCHECK2(bp->bp_chaddr[0], 6);
		ND_PRINT((ndo, "\n\t  Client-Ethernet-Address %s", etheraddr_string(ndo, bp->bp_chaddr)));
	}

	ND_TCHECK2(bp->bp_sname[0], 1);		/* check first char only */
	if (*bp->bp_sname) {
		ND_PRINT((ndo, "\n\t  sname \""));
		if (fn_printztn(ndo, bp->bp_sname, (u_int)sizeof bp->bp_sname,
		    ndo->ndo_snapend)) {
			ND_PRINT((ndo, "\""));
			ND_PRINT((ndo, "%s", tstr + 1));
			return;
		}
		ND_PRINT((ndo, "\""));
	}
	ND_TCHECK2(bp->bp_file[0], 1);		/* check first char only */
	if (*bp->bp_file) {
		ND_PRINT((ndo, "\n\t  file \""));
		if (fn_printztn(ndo, bp->bp_file, (u_int)sizeof bp->bp_file,
		    ndo->ndo_snapend)) {
			ND_PRINT((ndo, "\""));
			ND_PRINT((ndo, "%s", tstr + 1));
			return;
		}
		ND_PRINT((ndo, "\""));
	}

	/* Decode the vendor buffer */
	ND_TCHECK(bp->bp_vend[0]);
	if (memcmp((const char *)bp->bp_vend, vm_rfc1048,
		    sizeof(uint32_t)) == 0)
		rfc1048_print(ndo, bp->bp_vend);
	else if (memcmp((const char *)bp->bp_vend, vm_cmu,
			sizeof(uint32_t)) == 0)
		cmu_print(ndo, bp->bp_vend);
	else {
		uint32_t ul;

		ul = EXTRACT_32BITS(&bp->bp_vend);
		if (ul != 0)
			ND_PRINT((ndo, "\n\t  Vendor-#0x%x", ul));
	}

	return;
trunc:
	ND_PRINT((ndo, "%s", tstr));
}
