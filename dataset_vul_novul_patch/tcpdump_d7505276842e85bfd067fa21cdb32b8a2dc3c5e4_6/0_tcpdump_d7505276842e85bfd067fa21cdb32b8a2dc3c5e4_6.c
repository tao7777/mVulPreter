rpl_daoack_print(netdissect_options *ndo,
                 const u_char *bp, u_int length)
{
        const struct nd_rpl_daoack *daoack = (const struct nd_rpl_daoack *)bp;
        const char *dagid_str = "<elided>";

        ND_TCHECK2(*daoack, ND_RPL_DAOACK_MIN_LEN);
        if (length < ND_RPL_DAOACK_MIN_LEN)
        	goto tooshort;

        bp += ND_RPL_DAOACK_MIN_LEN;
        length -= ND_RPL_DAOACK_MIN_LEN;
        if(RPL_DAOACK_D(daoack->rpl_flags)) {
                ND_TCHECK2(daoack->rpl_dagid, DAGID_LEN);
                if (length < DAGID_LEN)
                	goto tooshort;
                dagid_str = ip6addr_string (ndo, daoack->rpl_dagid);
                bp += DAGID_LEN;
                length -= DAGID_LEN;
        }

        ND_PRINT((ndo, " [dagid:%s,seq:%u,instance:%u,status:%u]",
                  dagid_str,
                  daoack->rpl_daoseq,
                  daoack->rpl_instanceid,
                  daoack->rpl_status));

        /* no officially defined options for DAOACK, but print any we find */
        if(ndo->ndo_vflag > 1) {
                const struct rpl_dio_genoption *opt = (const struct rpl_dio_genoption *)bp;
                rpl_dio_printopt(ndo, opt, length);
        }
 	return;
 
 trunc:
	ND_PRINT((ndo, "%s", rpl_tstr));
 	return;
 
 tooshort:
	ND_PRINT((ndo," [|dao-length too short]"));
	return;
}
