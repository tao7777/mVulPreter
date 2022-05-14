IPV6BuildTestPacket(uint32_t id, uint16_t off, int mf, const char content,
    int content_len)
 {
     Packet *p = NULL;
     uint8_t *pcontent;
    IPV6Hdr ip6h;

    p = SCCalloc(1, sizeof(*p) + default_packet_size);
    if (unlikely(p == NULL))
        return NULL;

    PACKET_INITIALIZE(p);

    gettimeofday(&p->ts, NULL);

    ip6h.s_ip6_nxt = 44;
    ip6h.s_ip6_hlim = 2;

    /* Source and dest address - very bogus addresses. */
    ip6h.s_ip6_src[0] = 0x01010101;
    ip6h.s_ip6_src[1] = 0x01010101;
    ip6h.s_ip6_src[2] = 0x01010101;
    ip6h.s_ip6_src[3] = 0x01010101;
    ip6h.s_ip6_dst[0] = 0x02020202;
    ip6h.s_ip6_dst[1] = 0x02020202;
    ip6h.s_ip6_dst[2] = 0x02020202;
    ip6h.s_ip6_dst[3] = 0x02020202;

    /* copy content_len crap, we need full length */
    PacketCopyData(p, (uint8_t *)&ip6h, sizeof(IPV6Hdr));

    p->ip6h = (IPV6Hdr *)GET_PKT_DATA(p);
     IPV6_SET_RAW_VER(p->ip6h, 6);
     /* Fragmentation header. */
     IPV6FragHdr *fh = (IPV6FragHdr *)(GET_PKT_DATA(p) + sizeof(IPV6Hdr));
    fh->ip6fh_nxt = IPPROTO_ICMP;
     fh->ip6fh_ident = htonl(id);
     fh->ip6fh_offlg = htons((off << 3) | mf);
 
    DecodeIPV6FragHeader(p, (uint8_t *)fh, 8, 8 + content_len, 0);

    pcontent = SCCalloc(1, content_len);
    if (unlikely(pcontent == NULL))
        return NULL;
    memset(pcontent, content, content_len);
    PacketCopyDataOffset(p, sizeof(IPV6Hdr) + sizeof(IPV6FragHdr), pcontent, content_len);
    SET_PKT_LEN(p, sizeof(IPV6Hdr) + sizeof(IPV6FragHdr) + content_len);
    SCFree(pcontent);

    p->ip6h->s_ip6_plen = htons(sizeof(IPV6FragHdr) + content_len);

    SET_IPV6_SRC_ADDR(p, &p->src);
    SET_IPV6_DST_ADDR(p, &p->dst);

    /* Self test. */
    if (IPV6_GET_VER(p) != 6)
        goto error;
    if (IPV6_GET_NH(p) != 44)
        goto error;
    if (IPV6_GET_PLEN(p) != sizeof(IPV6FragHdr) + content_len)
        goto error;

    return p;
error:
    fprintf(stderr, "Error building test packet.\n");
    if (p != NULL)
        SCFree(p);
    return NULL;
}
