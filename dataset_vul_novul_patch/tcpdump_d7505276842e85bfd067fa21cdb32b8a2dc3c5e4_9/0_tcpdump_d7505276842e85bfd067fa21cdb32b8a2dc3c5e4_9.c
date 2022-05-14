rpl_print(netdissect_options *ndo,
          const struct icmp6_hdr *hdr,
          const u_char *bp, u_int length)
{
        int secured = hdr->icmp6_code & 0x80;
        int basecode= hdr->icmp6_code & 0x7f;

        if(secured) {
                ND_PRINT((ndo, ", (SEC) [worktodo]"));
                /* XXX
                 * the next header pointer needs to move forward to
                 * skip the secure part.
                 */
                return;
        } else {
                ND_PRINT((ndo, ", (CLR)"));
        }

        switch(basecode) {
        case ND_RPL_DAG_IS:
                ND_PRINT((ndo, "DODAG Information Solicitation"));
                if(ndo->ndo_vflag) {
                }
                break;
        case ND_RPL_DAG_IO:
                ND_PRINT((ndo, "DODAG Information Object"));
                if(ndo->ndo_vflag) {
                        rpl_dio_print(ndo, bp, length);
                }
                break;
        case ND_RPL_DAO:
                ND_PRINT((ndo, "Destination Advertisement Object"));
                if(ndo->ndo_vflag) {
                        rpl_dao_print(ndo, bp, length);
                }
                break;
        case ND_RPL_DAO_ACK:
                ND_PRINT((ndo, "Destination Advertisement Object Ack"));
                if(ndo->ndo_vflag) {
                        rpl_daoack_print(ndo, bp, length);
                }
                break;
        default:
                ND_PRINT((ndo, "RPL message, unknown code %u",hdr->icmp6_code));
                break;
        }
	return;
 
 #if 0
 trunc:
	ND_PRINT((ndo, "%s", rpl_tstr));
 	return;
 #endif
 
}
