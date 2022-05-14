print_decnet_ctlmsg(netdissect_options *ndo,
                     register const union routehdr *rhp, u_int length,
                     u_int caplen)
 {
 	int mflags = EXTRACT_LE_8BITS(rhp->rh_short.sh_flags);
 	register const union controlmsg *cmp = (const union controlmsg *)rhp;
 	int src, dst, info, blksize, eco, ueco, hello, other, vers;
	etheraddr srcea, rtea;
	int priority;
	const char *rhpx = (const char *)rhp;
	int ret;

	switch (mflags & RMF_CTLMASK) {
	case RMF_INIT:
	    ND_PRINT((ndo, "init "));
	    if (length < sizeof(struct initmsg))
		goto trunc;
	    ND_TCHECK(cmp->cm_init);
	    src = EXTRACT_LE_16BITS(cmp->cm_init.in_src);
	    info = EXTRACT_LE_8BITS(cmp->cm_init.in_info);
	    blksize = EXTRACT_LE_16BITS(cmp->cm_init.in_blksize);
	    vers = EXTRACT_LE_8BITS(cmp->cm_init.in_vers);
	    eco = EXTRACT_LE_8BITS(cmp->cm_init.in_eco);
	    ueco = EXTRACT_LE_8BITS(cmp->cm_init.in_ueco);
	    hello = EXTRACT_LE_16BITS(cmp->cm_init.in_hello);
	    print_t_info(ndo, info);
	    ND_PRINT((ndo,
		"src %sblksize %d vers %d eco %d ueco %d hello %d",
			dnaddr_string(ndo, src), blksize, vers, eco, ueco,
			hello));
	    ret = 1;
	    break;
	case RMF_VER:
	    ND_PRINT((ndo, "verification "));
	    if (length < sizeof(struct verifmsg))
		goto trunc;
	    ND_TCHECK(cmp->cm_ver);
	    src = EXTRACT_LE_16BITS(cmp->cm_ver.ve_src);
	    other = EXTRACT_LE_8BITS(cmp->cm_ver.ve_fcnval);
	    ND_PRINT((ndo, "src %s fcnval %o", dnaddr_string(ndo, src), other));
	    ret = 1;
	    break;
	case RMF_TEST:
	    ND_PRINT((ndo, "test "));
	    if (length < sizeof(struct testmsg))
		goto trunc;
	    ND_TCHECK(cmp->cm_test);
	    src = EXTRACT_LE_16BITS(cmp->cm_test.te_src);
	    other = EXTRACT_LE_8BITS(cmp->cm_test.te_data);
	    ND_PRINT((ndo, "src %s data %o", dnaddr_string(ndo, src), other));
	    ret = 1;
	    break;
	case RMF_L1ROUT:
	    ND_PRINT((ndo, "lev-1-routing "));
	    if (length < sizeof(struct l1rout))
		goto trunc;
	    ND_TCHECK(cmp->cm_l1rou);
	    src = EXTRACT_LE_16BITS(cmp->cm_l1rou.r1_src);
	    ND_PRINT((ndo, "src %s ", dnaddr_string(ndo, src)));
	    ret = print_l1_routes(ndo, &(rhpx[sizeof(struct l1rout)]),
				length - sizeof(struct l1rout));
	    break;
	case RMF_L2ROUT:
	    ND_PRINT((ndo, "lev-2-routing "));
	    if (length < sizeof(struct l2rout))
		goto trunc;
	    ND_TCHECK(cmp->cm_l2rout);
	    src = EXTRACT_LE_16BITS(cmp->cm_l2rout.r2_src);
	    ND_PRINT((ndo, "src %s ", dnaddr_string(ndo, src)));
	    ret = print_l2_routes(ndo, &(rhpx[sizeof(struct l2rout)]),
				length - sizeof(struct l2rout));
	    break;
	case RMF_RHELLO:
	    ND_PRINT((ndo, "router-hello "));
	    if (length < sizeof(struct rhellomsg))
		goto trunc;
	    ND_TCHECK(cmp->cm_rhello);
	    vers = EXTRACT_LE_8BITS(cmp->cm_rhello.rh_vers);
	    eco = EXTRACT_LE_8BITS(cmp->cm_rhello.rh_eco);
	    ueco = EXTRACT_LE_8BITS(cmp->cm_rhello.rh_ueco);
	    memcpy((char *)&srcea, (const char *)&(cmp->cm_rhello.rh_src),
		sizeof(srcea));
	    src = EXTRACT_LE_16BITS(srcea.dne_remote.dne_nodeaddr);
	    info = EXTRACT_LE_8BITS(cmp->cm_rhello.rh_info);
	    blksize = EXTRACT_LE_16BITS(cmp->cm_rhello.rh_blksize);
	    priority = EXTRACT_LE_8BITS(cmp->cm_rhello.rh_priority);
	    hello = EXTRACT_LE_16BITS(cmp->cm_rhello.rh_hello);
	    print_i_info(ndo, info);
	    ND_PRINT((ndo,
	    "vers %d eco %d ueco %d src %s blksize %d pri %d hello %d",
			vers, eco, ueco, dnaddr_string(ndo, src),
			blksize, priority, hello));
	    ret = print_elist(&(rhpx[sizeof(struct rhellomsg)]),
				length - sizeof(struct rhellomsg));
	    break;
	case RMF_EHELLO:
	    ND_PRINT((ndo, "endnode-hello "));
	    if (length < sizeof(struct ehellomsg))
		goto trunc;
	    ND_TCHECK(cmp->cm_ehello);
	    vers = EXTRACT_LE_8BITS(cmp->cm_ehello.eh_vers);
	    eco = EXTRACT_LE_8BITS(cmp->cm_ehello.eh_eco);
	    ueco = EXTRACT_LE_8BITS(cmp->cm_ehello.eh_ueco);
	    memcpy((char *)&srcea, (const char *)&(cmp->cm_ehello.eh_src),
		sizeof(srcea));
	    src = EXTRACT_LE_16BITS(srcea.dne_remote.dne_nodeaddr);
	    info = EXTRACT_LE_8BITS(cmp->cm_ehello.eh_info);
	    blksize = EXTRACT_LE_16BITS(cmp->cm_ehello.eh_blksize);
	    /*seed*/
	    memcpy((char *)&rtea, (const char *)&(cmp->cm_ehello.eh_router),
		sizeof(rtea));
	    dst = EXTRACT_LE_16BITS(rtea.dne_remote.dne_nodeaddr);
	    hello = EXTRACT_LE_16BITS(cmp->cm_ehello.eh_hello);
	    other = EXTRACT_LE_8BITS(cmp->cm_ehello.eh_data);
	    print_i_info(ndo, info);
	    ND_PRINT((ndo,
	"vers %d eco %d ueco %d src %s blksize %d rtr %s hello %d data %o",
			vers, eco, ueco, dnaddr_string(ndo, src),
			blksize, dnaddr_string(ndo, dst), hello, other));
	    ret = 1;
	    break;

	default:
	    ND_PRINT((ndo, "unknown control message"));
	    ND_DEFAULTPRINT((const u_char *)rhp, min(length, caplen));
	    ret = 1;
	    break;
	}
	return (ret);

trunc:
	return (0);
}
