BuildTestPacket(uint16_t id, uint16_t off, int mf, const char content,
    int content_len)
 {
     Packet *p = NULL;
     int hlen = 20;
    int ttl = 64;
    uint8_t *pcontent;
    IPV4Hdr ip4h;

    p = SCCalloc(1, sizeof(*p) + default_packet_size);
    if (unlikely(p == NULL))
        return NULL;

    PACKET_INITIALIZE(p);

    gettimeofday(&p->ts, NULL);
    ip4h.ip_verhl = 4 << 4;
    ip4h.ip_verhl |= hlen >> 2;
    ip4h.ip_len = htons(hlen + content_len);
    ip4h.ip_id = htons(id);
    ip4h.ip_off = htons(off);
    if (mf)
        ip4h.ip_off = htons(IP_MF | off);
     else
         ip4h.ip_off = htons(off);
     ip4h.ip_ttl = ttl;
    ip4h.ip_proto = IPPROTO_ICMP;
 
     ip4h.s_ip_src.s_addr = 0x01010101; /* 1.1.1.1 */
     ip4h.s_ip_dst.s_addr = 0x02020202; /* 2.2.2.2 */

    /* copy content_len crap, we need full length */
    PacketCopyData(p, (uint8_t *)&ip4h, sizeof(ip4h));
    p->ip4h = (IPV4Hdr *)GET_PKT_DATA(p);
    SET_IPV4_SRC_ADDR(p, &p->src);
    SET_IPV4_DST_ADDR(p, &p->dst);

    pcontent = SCCalloc(1, content_len);
    if (unlikely(pcontent == NULL))
        return NULL;
    memset(pcontent, content, content_len);
    PacketCopyDataOffset(p, hlen, pcontent, content_len);
    SET_PKT_LEN(p, hlen + content_len);
    SCFree(pcontent);

    p->ip4h->ip_csum = IPV4CalculateChecksum((uint16_t *)GET_PKT_DATA(p), hlen);

    /* Self test. */
    if (IPV4_GET_VER(p) != 4)
        goto error;
    if (IPV4_GET_HLEN(p) != hlen)
        goto error;
    if (IPV4_GET_IPLEN(p) != hlen + content_len)
        goto error;
    if (IPV4_GET_IPID(p) != id)
        goto error;
    if (IPV4_GET_IPOFFSET(p) != off)
        goto error;
    if (IPV4_GET_MF(p) != mf)
         goto error;
     if (IPV4_GET_IPTTL(p) != ttl)
         goto error;
    if (IPV4_GET_IPPROTO(p) != IPPROTO_ICMP)
         goto error;
 
     return p;
error:
    if (p != NULL)
        SCFree(p);
    return NULL;
}
