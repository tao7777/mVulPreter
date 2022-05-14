isis_print_is_reach_subtlv(netdissect_options *ndo,
                           const uint8_t *tptr, u_int subt, u_int subl,
                           const char *ident)
{
        u_int te_class,priority_level,gmpls_switch_cap;
        union { /* int to float conversion buffer for several subTLVs */
            float f;
            uint32_t i;
        } bw;

        /* first lets see if we know the subTLVs name*/
	ND_PRINT((ndo, "%s%s subTLV #%u, length: %u",
	          ident, tok2str(isis_ext_is_reach_subtlv_values, "unknown", subt),
	          subt, subl));

	ND_TCHECK2(*tptr, subl);

        switch(subt) {
        case ISIS_SUBTLV_EXT_IS_REACH_ADMIN_GROUP:
        case ISIS_SUBTLV_EXT_IS_REACH_LINK_LOCAL_REMOTE_ID:
        case ISIS_SUBTLV_EXT_IS_REACH_LINK_REMOTE_ID:
	    if (subl >= 4) {
	      ND_PRINT((ndo, ", 0x%08x", EXTRACT_32BITS(tptr)));
	      if (subl == 8) /* rfc4205 */
	        ND_PRINT((ndo, ", 0x%08x", EXTRACT_32BITS(tptr+4)));
	    }
	    break;
        case ISIS_SUBTLV_EXT_IS_REACH_IPV4_INTF_ADDR:
        case ISIS_SUBTLV_EXT_IS_REACH_IPV4_NEIGHBOR_ADDR:
            if (subl >= sizeof(struct in_addr))
              ND_PRINT((ndo, ", %s", ipaddr_string(ndo, tptr)));
            break;
        case ISIS_SUBTLV_EXT_IS_REACH_MAX_LINK_BW :
	case ISIS_SUBTLV_EXT_IS_REACH_RESERVABLE_BW:
            if (subl >= 4) {
              bw.i = EXTRACT_32BITS(tptr);
              ND_PRINT((ndo, ", %.3f Mbps", bw.f * 8 / 1000000));
            }
            break;
        case ISIS_SUBTLV_EXT_IS_REACH_UNRESERVED_BW :
            if (subl >= 32) {
              for (te_class = 0; te_class < 8; te_class++) {
                bw.i = EXTRACT_32BITS(tptr);
                ND_PRINT((ndo, "%s  TE-Class %u: %.3f Mbps",
                       ident,
                       te_class,
                       bw.f * 8 / 1000000));
		tptr+=4;
	      }
            }
             break;
         case ISIS_SUBTLV_EXT_IS_REACH_BW_CONSTRAINTS: /* fall through */
         case ISIS_SUBTLV_EXT_IS_REACH_BW_CONSTRAINTS_OLD:
            if (subl == 0)
                break;
             ND_PRINT((ndo, "%sBandwidth Constraints Model ID: %s (%u)",
                    ident,
                    tok2str(diffserv_te_bc_values, "unknown", *tptr),
                    *tptr));
             tptr++;
             /* decode BCs until the subTLV ends */
             for (te_class = 0; te_class < (subl-1)/4; te_class++) {
                 bw.i = EXTRACT_32BITS(tptr);
                 ND_PRINT((ndo, "%s  Bandwidth constraint CT%u: %.3f Mbps",
                        ident,
                       te_class,
                       bw.f * 8 / 1000000));
		tptr+=4;
            }
            break;
        case ISIS_SUBTLV_EXT_IS_REACH_TE_METRIC:
            if (subl >= 3)
              ND_PRINT((ndo, ", %u", EXTRACT_24BITS(tptr)));
            break;
        case ISIS_SUBTLV_EXT_IS_REACH_LINK_ATTRIBUTE:
            if (subl == 2) {
               ND_PRINT((ndo, ", [ %s ] (0x%04x)",
                      bittok2str(isis_subtlv_link_attribute_values,
                                 "Unknown",
                                 EXTRACT_16BITS(tptr)),
                      EXTRACT_16BITS(tptr)));
            }
            break;
        case ISIS_SUBTLV_EXT_IS_REACH_LINK_PROTECTION_TYPE:
            if (subl >= 2) {
              ND_PRINT((ndo, ", %s, Priority %u",
		   bittok2str(gmpls_link_prot_values, "none", *tptr),
                   *(tptr+1)));
            }
            break;
        case ISIS_SUBTLV_SPB_METRIC:
            if (subl >= 6) {
              ND_PRINT((ndo, ", LM: %u", EXTRACT_24BITS(tptr)));
              tptr=tptr+3;
              ND_PRINT((ndo, ", P: %u", *(tptr)));
              tptr++;
              ND_PRINT((ndo, ", P-ID: %u", EXTRACT_16BITS(tptr)));
            }
            break;
        case ISIS_SUBTLV_EXT_IS_REACH_INTF_SW_CAP_DESCR:
            if (subl >= 36) {
              gmpls_switch_cap = *tptr;
              ND_PRINT((ndo, "%s  Interface Switching Capability:%s",
                   ident,
                   tok2str(gmpls_switch_cap_values, "Unknown", gmpls_switch_cap)));
              ND_PRINT((ndo, ", LSP Encoding: %s",
                   tok2str(gmpls_encoding_values, "Unknown", *(tptr + 1))));
	      tptr+=4;
              ND_PRINT((ndo, "%s  Max LSP Bandwidth:", ident));
              for (priority_level = 0; priority_level < 8; priority_level++) {
                bw.i = EXTRACT_32BITS(tptr);
                ND_PRINT((ndo, "%s    priority level %d: %.3f Mbps",
                       ident,
                       priority_level,
                       bw.f * 8 / 1000000));
		tptr+=4;
              }
              subl-=36;
              switch (gmpls_switch_cap) {
              case GMPLS_PSC1:
              case GMPLS_PSC2:
              case GMPLS_PSC3:
              case GMPLS_PSC4:
                ND_TCHECK2(*tptr, 6);
                bw.i = EXTRACT_32BITS(tptr);
                ND_PRINT((ndo, "%s  Min LSP Bandwidth: %.3f Mbps", ident, bw.f * 8 / 1000000));
                ND_PRINT((ndo, "%s  Interface MTU: %u", ident, EXTRACT_16BITS(tptr + 4)));
                break;
              case GMPLS_TSC:
                ND_TCHECK2(*tptr, 8);
                bw.i = EXTRACT_32BITS(tptr);
                ND_PRINT((ndo, "%s  Min LSP Bandwidth: %.3f Mbps", ident, bw.f * 8 / 1000000));
                ND_PRINT((ndo, "%s  Indication %s", ident,
                       tok2str(gmpls_switch_cap_tsc_indication_values, "Unknown (%u)", *(tptr + 4))));
                break;
              default:
                /* there is some optional stuff left to decode but this is as of yet
                   not specified so just lets hexdump what is left */
                if(subl>0){
                  if (!print_unknown_data(ndo, tptr, "\n\t\t    ", subl))
                    return(0);
                }
              }
            }
            break;
        default:
            if (!print_unknown_data(ndo, tptr, "\n\t\t    ", subl))
                return(0);
            break;
        }
        return(1);

trunc:
    return(0);
}
