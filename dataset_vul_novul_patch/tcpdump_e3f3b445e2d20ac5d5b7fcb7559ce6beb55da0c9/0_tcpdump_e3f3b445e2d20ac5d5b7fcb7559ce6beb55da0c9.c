bgp_capabilities_print(netdissect_options *ndo,
                       const u_char *opt, int caps_len)
{
	int cap_type, cap_len, tcap_len, cap_offset;
        int i = 0;

        while (i < caps_len) {
                ND_TCHECK2(opt[i], BGP_CAP_HEADER_SIZE);
                cap_type=opt[i];
                cap_len=opt[i+1];
                tcap_len=cap_len;
                ND_PRINT((ndo, "\n\t      %s (%u), length: %u",
                       tok2str(bgp_capcode_values, "Unknown",
                                  cap_type),
                       cap_type,
                       cap_len));
                 ND_TCHECK2(opt[i+2], cap_len);
                 switch (cap_type) {
                 case BGP_CAPCODE_MP:
                    /* AFI (16 bits), Reserved (8 bits), SAFI (8 bits) */
                    ND_TCHECK_8BITS(opt + i + 5);
                     ND_PRINT((ndo, "\n\t\tAFI %s (%u), SAFI %s (%u)",
                            tok2str(af_values, "Unknown",
                                       EXTRACT_16BITS(opt+i+2)),
                           EXTRACT_16BITS(opt+i+2),
                           tok2str(bgp_safi_values, "Unknown",
                                      opt[i+5]),
                           opt[i+5]));
                    break;
                case BGP_CAPCODE_RESTART:
                    /* Restart Flags (4 bits), Restart Time in seconds (12 bits) */
                    ND_TCHECK_16BITS(opt + i + 2);
                    ND_PRINT((ndo, "\n\t\tRestart Flags: [%s], Restart Time %us",
                           ((opt[i+2])&0x80) ? "R" : "none",
                           EXTRACT_16BITS(opt+i+2)&0xfff));
                    tcap_len-=2;
                    cap_offset=4;
                    while(tcap_len>=4) {
                        ND_PRINT((ndo, "\n\t\t  AFI %s (%u), SAFI %s (%u), Forwarding state preserved: %s",
                               tok2str(af_values,"Unknown",
                                          EXTRACT_16BITS(opt+i+cap_offset)),
                               EXTRACT_16BITS(opt+i+cap_offset),
                               tok2str(bgp_safi_values,"Unknown",
                                          opt[i+cap_offset+2]),
                               opt[i+cap_offset+2],
                               ((opt[i+cap_offset+3])&0x80) ? "yes" : "no" ));
                        tcap_len-=4;
                        cap_offset+=4;
                    }
                    break;
                case BGP_CAPCODE_RR:
                case BGP_CAPCODE_RR_CISCO:
                    break;
                case BGP_CAPCODE_AS_NEW:

                    /*
                     * Extract the 4 byte AS number encoded.
                     */
                    if (cap_len == 4) {
                        ND_PRINT((ndo, "\n\t\t 4 Byte AS %s",
                            as_printf(ndo, astostr, sizeof(astostr),
                            EXTRACT_32BITS(opt + i + 2))));
                    }
                    break;
                case BGP_CAPCODE_ADD_PATH:
                    cap_offset=2;
                    if (tcap_len == 0) {
                        ND_PRINT((ndo, " (bogus)")); /* length */
                        break;
                    }
                    while (tcap_len > 0) {
                        if (tcap_len < 4) {
                            ND_PRINT((ndo, "\n\t\t(invalid)"));
                            break;
                        }
                        ND_PRINT((ndo, "\n\t\tAFI %s (%u), SAFI %s (%u), Send/Receive: %s",
                                  tok2str(af_values,"Unknown",EXTRACT_16BITS(opt+i+cap_offset)),
                                  EXTRACT_16BITS(opt+i+cap_offset),
                                  tok2str(bgp_safi_values,"Unknown",opt[i+cap_offset+2]),
                                  opt[i+cap_offset+2],
                                  tok2str(bgp_add_path_recvsend,"Bogus (0x%02x)",opt[i+cap_offset+3])
                        ));
                        tcap_len-=4;
                        cap_offset+=4;
                    }
                    break;
                default:
                    ND_PRINT((ndo, "\n\t\tno decoder for Capability %u",
                           cap_type));
                    if (ndo->ndo_vflag <= 1)
                        print_unknown_data(ndo, &opt[i+2], "\n\t\t", cap_len);
                    break;
                }
                if (ndo->ndo_vflag > 1 && cap_len > 0) {
                    print_unknown_data(ndo, &opt[i+2], "\n\t\t", cap_len);
                }
                i += BGP_CAP_HEADER_SIZE + cap_len;
        }
        return;

trunc:
	ND_PRINT((ndo, "[|BGP]"));
}
