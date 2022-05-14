int DecodeTunnel(ThreadVars *tv, DecodeThreadVars *dtv, Packet *p,
        uint8_t *pkt, uint32_t len, PacketQueue *pq, enum DecodeTunnelProto proto)
{
    switch (proto) {
        case DECODE_TUNNEL_PPP:
            return DecodePPP(tv, dtv, p, pkt, len, pq);
         case DECODE_TUNNEL_IPV4:
             return DecodeIPV4(tv, dtv, p, pkt, len, pq);
         case DECODE_TUNNEL_IPV6:
        case DECODE_TUNNEL_IPV6_TEREDO:
             return DecodeIPV6(tv, dtv, p, pkt, len, pq);
         case DECODE_TUNNEL_VLAN:
             return DecodeVLAN(tv, dtv, p, pkt, len, pq);
        case DECODE_TUNNEL_ETHERNET:
            return DecodeEthernet(tv, dtv, p, pkt, len, pq);
         case DECODE_TUNNEL_ERSPAN:
             return DecodeERSPAN(tv, dtv, p, pkt, len, pq);
         default:
            SCLogDebug("FIXME: DecodeTunnel: protocol %" PRIu32 " not supported.", proto);
             break;
     }
     return TM_ECODE_OK;
}
