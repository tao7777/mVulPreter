Packet *PacketTunnelPktSetup(ThreadVars *tv, DecodeThreadVars *dtv, Packet *parent,
                             uint8_t *pkt, uint32_t len, enum DecodeTunnelProto proto,
                             PacketQueue *pq)
{
    int ret;

    SCEnter();

    /* get us a packet */
    Packet *p = PacketGetFromQueueOrAlloc();
    if (unlikely(p == NULL)) {
        SCReturnPtr(NULL, "Packet");
    }

    /* copy packet and set lenght, proto */
    PacketCopyData(p, pkt, len);
    p->recursion_level = parent->recursion_level + 1;
    p->ts.tv_sec = parent->ts.tv_sec;
    p->ts.tv_usec = parent->ts.tv_usec;
    p->datalink = DLT_RAW;
    p->tenant_id = parent->tenant_id;

    /* set the root ptr to the lowest layer */
    if (parent->root != NULL)
        p->root = parent->root;
    else
        p->root = parent;

    /* tell new packet it's part of a tunnel */
    SET_TUNNEL_PKT(p);

     ret = DecodeTunnel(tv, dtv, p, GET_PKT_DATA(p),
                        GET_PKT_LEN(p), pq, proto);
 
    if (unlikely(ret != TM_ECODE_OK) ||
            (proto == DECODE_TUNNEL_IPV6_TEREDO && (p->flags & PKT_IS_INVALID)))
    {
        /* Not a (valid) tunnel packet */
        SCLogDebug("tunnel packet is invalid");

         p->root = NULL;
         UNSET_TUNNEL_PKT(p);
         TmqhOutputPacketpool(tv, p);
        SCReturnPtr(NULL, "Packet");
    }


    /* tell parent packet it's part of a tunnel */
    SET_TUNNEL_PKT(parent);

    /* increment tunnel packet refcnt in the root packet */
    TUNNEL_INCR_PKT_TPR(p);

    /* disable payload (not packet) inspection on the parent, as the payload
     * is the packet we will now run through the system separately. We do
     * check it against the ip/port/other header checks though */
    DecodeSetNoPayloadInspectionFlag(parent);
    SCReturnPtr(p, "Packet");
}
