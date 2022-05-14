print_prefix(netdissect_options *ndo, const u_char *prefix, u_int max_length)
{
    int plenbytes;
    char buf[sizeof("xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx::/128")];

    if (prefix[0] >= 96 && max_length >= IPV4_MAPPED_HEADING_LEN + 1 &&
        is_ipv4_mapped_address(&prefix[1])) {
        struct in_addr addr;
        u_int plen;

        plen = prefix[0]-96;
        if (32 < plen)
            return -1;
        max_length -= 1;

        memset(&addr, 0, sizeof(addr));
        plenbytes = (plen + 7) / 8;
        if (max_length < (u_int)plenbytes + IPV4_MAPPED_HEADING_LEN)
            return -3;
        memcpy(&addr, &prefix[1 + IPV4_MAPPED_HEADING_LEN], plenbytes);
        if (plen % 8) {
		((u_char *)&addr)[plenbytes - 1] &=
			((0xff00 >> (plen % 8)) & 0xff);
	}
	snprintf(buf, sizeof(buf), "%s/%d", ipaddr_string(ndo, &addr), plen);
         plenbytes += 1 + IPV4_MAPPED_HEADING_LEN;
     } else {
         plenbytes = decode_prefix6(ndo, prefix, max_length, buf, sizeof(buf));
        if (plenbytes < 0)
            return plenbytes;
     }
 
     ND_PRINT((ndo, "%s", buf));
    return plenbytes;
}
