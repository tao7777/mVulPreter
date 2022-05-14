dhcpv6_print(netdissect_options *ndo,
             const u_char *cp, u_int length, int indent)
{
    u_int i, t;
    const u_char *tlv, *value;
    uint16_t type, optlen;
 
     i = 0;
     while (i < length) {
        if (i + 4 > length)
            return -1;
         tlv = cp + i;
         type = EXTRACT_16BITS(tlv);
         optlen = EXTRACT_16BITS(tlv + 2);
        value = tlv + 4;

        ND_PRINT((ndo, "\n"));
        for (t = indent; t > 0; t--)
            ND_PRINT((ndo, "\t"));
 
         ND_PRINT((ndo, "%s", tok2str(dh6opt_str, "Unknown", type)));
         ND_PRINT((ndo," (%u)", optlen + 4 ));
        if (i + 4 + optlen > length)
            return -1;
 
         switch (type) {
             case DH6OPT_DNS_SERVERS:
            case DH6OPT_SNTP_SERVERS: {
                if (optlen % 16 != 0) {
                    ND_PRINT((ndo, " %s", istr));
                    return -1;
                }
                for (t = 0; t < optlen; t += 16)
                    ND_PRINT((ndo, " %s", ip6addr_string(ndo, value + t)));
            }
                break;
            case DH6OPT_DOMAIN_LIST: {
                const u_char *tp = value;
                while (tp < value + optlen) {
                    ND_PRINT((ndo, " "));
                    if ((tp = ns_nprint(ndo, tp, value + optlen)) == NULL)
                        return -1;
                }
            }
                break;
        }

        i += 4 + optlen;
    }
    return 0;
}
