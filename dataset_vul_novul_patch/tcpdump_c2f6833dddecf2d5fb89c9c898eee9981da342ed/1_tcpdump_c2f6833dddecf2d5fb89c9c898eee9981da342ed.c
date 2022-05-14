dhcpv4_print(netdissect_options *ndo,
             const u_char *cp, u_int length, int indent)
{
    u_int i, t;
    const u_char *tlv, *value;
    uint8_t type, optlen;
 
     i = 0;
     while (i < length) {
         tlv = cp + i;
         type = (uint8_t)tlv[0];
         optlen = (uint8_t)tlv[1];
        value = tlv + 2;

        ND_PRINT((ndo, "\n"));
        for (t = indent; t > 0; t--)
            ND_PRINT((ndo, "\t"));
 
         ND_PRINT((ndo, "%s", tok2str(dh4opt_str, "Unknown", type)));
         ND_PRINT((ndo," (%u)", optlen + 2 ));
 
         switch (type) {
         case DH4OPT_DNS_SERVERS:
        case DH4OPT_NTP_SERVERS: {
            if (optlen < 4 || optlen % 4 != 0) {
                return -1;
            }
            for (t = 0; t < optlen; t += 4)
                ND_PRINT((ndo, " %s", ipaddr_string(ndo, value + t)));
        }
            break;
        case DH4OPT_DOMAIN_SEARCH: {
            const u_char *tp = value;
            while (tp < value + optlen) {
                ND_PRINT((ndo, " "));
                if ((tp = ns_nprint(ndo, tp, value + optlen)) == NULL)
                    return -1;
            }
        }
            break;
        }

        i += 2 + optlen;
    }
    return 0;
}
