rpl_dio_print(netdissect_options *ndo,
              const u_char *bp, u_int length)
{
        const struct nd_rpl_dio *dio = (const struct nd_rpl_dio *)bp;
        const char *dagid_str;

        ND_TCHECK(*dio);
        dagid_str = ip6addr_string (ndo, dio->rpl_dagid);

        ND_PRINT((ndo, " [dagid:%s,seq:%u,instance:%u,rank:%u,%smop:%s,prf:%u]",
                  dagid_str,
                  dio->rpl_dtsn,
                  dio->rpl_instanceid,
                  EXTRACT_16BITS(&dio->rpl_dagrank),
                  RPL_DIO_GROUNDED(dio->rpl_mopprf) ? "grounded,":"",
                  tok2str(rpl_mop_values, "mop%u", RPL_DIO_MOP(dio->rpl_mopprf)),
                  RPL_DIO_PRF(dio->rpl_mopprf)));

        if(ndo->ndo_vflag > 1) {
                const struct rpl_dio_genoption *opt = (const struct rpl_dio_genoption *)&dio[1];
                rpl_dio_printopt(ndo, opt, length);
         }
 	return;
 trunc:
	ND_PRINT((ndo, "%s", rpl_tstr));
 	return;
 }
