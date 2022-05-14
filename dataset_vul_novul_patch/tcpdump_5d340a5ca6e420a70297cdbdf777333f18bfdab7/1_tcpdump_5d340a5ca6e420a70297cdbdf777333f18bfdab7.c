 cfm_network_addr_print(netdissect_options *ndo,
                       register const u_char *tptr)
 {
     u_int network_addr_type;
     u_int hexdump =  FALSE;

    /*
     * Altough AFIs are tpically 2 octects wide,
      * 802.1ab specifies that this field width
      * is only once octet
      */
     network_addr_type = *tptr;
     ND_PRINT((ndo, "\n\t  Network Address Type %s (%u)",
            tok2str(af_values, "Unknown", network_addr_type),
           network_addr_type));

    /*
     * Resolve the passed in Address.
      */
     switch(network_addr_type) {
     case AFNUM_INET:
         ND_PRINT((ndo, ", %s", ipaddr_string(ndo, tptr + 1)));
         break;
 
     case AFNUM_INET6:
         ND_PRINT((ndo, ", %s", ip6addr_string(ndo, tptr + 1)));
         break;
 
    default:
        hexdump = TRUE;
        break;
    }

    return hexdump;
}
