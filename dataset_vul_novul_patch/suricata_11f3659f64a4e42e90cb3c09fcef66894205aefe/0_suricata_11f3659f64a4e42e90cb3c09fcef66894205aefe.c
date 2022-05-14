DecodeIPV6ExtHdrs(ThreadVars *tv, DecodeThreadVars *dtv, Packet *p, uint8_t *pkt, uint16_t len, PacketQueue *pq)
{
     SCEnter();
 
     uint8_t *orig_pkt = pkt;
    uint8_t nh = IPV6_GET_NH(p); /* careful, 0 is actually a real type */
     uint16_t hdrextlen = 0;
    uint16_t plen = len;
     char dstopts = 0;
     char exthdr_fh_done = 0;
     int hh = 0;
     int rh = 0;
     int eh = 0;
     int ah = 0;
 
     while(1)
     {
        if (nh == IPPROTO_NONE) {
            if (plen > 0) {
                /* No upper layer, but we do have data. Suspicious. */
                ENGINE_SET_EVENT(p, IPV6_DATA_AFTER_NONE_HEADER);
            }
             SCReturn;
         }
 
         if (plen < 2) { /* minimal needed in a hdr */
            ENGINE_SET_INVALID_EVENT(p, IPV6_TRUNC_EXTHDR);
             SCReturn;
         }
 
        switch(nh)
        {
            case IPPROTO_TCP:
                IPV6_SET_L4PROTO(p,nh);
                DecodeTCP(tv, dtv, p, pkt, plen, pq);
                SCReturn;

            case IPPROTO_UDP:
                IPV6_SET_L4PROTO(p,nh);
                DecodeUDP(tv, dtv, p, pkt, plen, pq);
                SCReturn;

            case IPPROTO_ICMPV6:
                IPV6_SET_L4PROTO(p,nh);
                DecodeICMPV6(tv, dtv, p, pkt, plen, pq);
                SCReturn;

            case IPPROTO_SCTP:
                IPV6_SET_L4PROTO(p,nh);
                DecodeSCTP(tv, dtv, p, pkt, plen, pq);
                SCReturn;

            case IPPROTO_ROUTING:
                IPV6_SET_L4PROTO(p,nh);
                hdrextlen = 8 + (*(pkt+1) * 8);  /* 8 bytes + length in 8 octet units */

                 SCLogDebug("hdrextlen %"PRIu8, hdrextlen);
 
                 if (hdrextlen > plen) {
                    ENGINE_SET_INVALID_EVENT(p, IPV6_TRUNC_EXTHDR);
                     SCReturn;
                 }
 
                if (rh) {
                    ENGINE_SET_EVENT(p, IPV6_EXTHDR_DUPL_RH);
                    /* skip past this extension so we can continue parsing the rest
                     * of the packet */
                    nh = *pkt;
                    pkt += hdrextlen;
                    plen -= hdrextlen;
                    break;
                }

                rh = 1;
                IPV6_EXTHDR_SET_RH(p);

                uint8_t ip6rh_type = *(pkt + 2);
                if (ip6rh_type == 0) {
                    ENGINE_SET_EVENT(p, IPV6_EXTHDR_RH_TYPE_0);
                }
                p->ip6eh.rh_type = ip6rh_type;

                nh = *pkt;
                pkt += hdrextlen;
                plen -= hdrextlen;
                break;

            case IPPROTO_HOPOPTS:
            case IPPROTO_DSTOPTS:
            {
                IPV6OptHAO hao_s, *hao = &hao_s;
                IPV6OptRA ra_s, *ra = &ra_s;
                IPV6OptJumbo jumbo_s, *jumbo = &jumbo_s;
                uint16_t optslen = 0;

                 IPV6_SET_L4PROTO(p,nh);
                 hdrextlen =  (*(pkt+1) + 1) << 3;
                 if (hdrextlen > plen) {
                    ENGINE_SET_INVALID_EVENT(p, IPV6_TRUNC_EXTHDR);
                     SCReturn;
                 }
 
                uint8_t *ptr = pkt + 2; /* +2 to go past nxthdr and len */

                /* point the pointers to right structures
                 * in Packet. */
                if (nh == IPPROTO_HOPOPTS) {
                    if (hh) {
                        ENGINE_SET_EVENT(p, IPV6_EXTHDR_DUPL_HH);
                        /* skip past this extension so we can continue parsing the rest
                         * of the packet */
                        nh = *pkt;
                        pkt += hdrextlen;
                        plen -= hdrextlen;
                        break;
                    }

                    hh = 1;

                    optslen = ((*(pkt + 1) + 1 ) << 3) - 2;
                }
                else if (nh == IPPROTO_DSTOPTS)
                {
                    if (dstopts == 0) {
                        optslen = ((*(pkt + 1) + 1 ) << 3) - 2;
                        dstopts = 1;
                    } else if (dstopts == 1) {
                        optslen = ((*(pkt + 1) + 1 ) << 3) - 2;
                        dstopts = 2;
                    } else {
                        ENGINE_SET_EVENT(p, IPV6_EXTHDR_DUPL_DH);
                        /* skip past this extension so we can continue parsing the rest
                         * of the packet */
                        nh = *pkt;
                        pkt += hdrextlen;
                        plen -= hdrextlen;
                        break;
                    }
                }

                 if (optslen > plen) {
                     /* since the packet is long enough (we checked
                      * plen against hdrlen, the optlen must be malformed. */
                    ENGINE_SET_INVALID_EVENT(p, IPV6_EXTHDR_INVALID_OPTLEN);
                     /* skip past this extension so we can continue parsing the rest
                      * of the packet */
                     nh = *pkt;
                    pkt += hdrextlen;
                    plen -= hdrextlen;
                    break;
                }
/** \todo move into own function to loaded on demand */
                uint16_t padn_cnt = 0;
                uint16_t other_cnt = 0;
                uint16_t offset = 0;
                while(offset < optslen)
                {
                    if (*ptr == IPV6OPT_PAD1)
                    {
                        padn_cnt++;
                        offset++;
                        ptr++;
                        continue;
                     }
 
                     if (offset + 1 >= optslen) {
                        ENGINE_SET_INVALID_EVENT(p, IPV6_EXTHDR_INVALID_OPTLEN);
                         break;
                     }
 
                    /* length field for each opt */
                    uint8_t ip6_optlen = *(ptr + 1);
 
                     /* see if the optlen from the packet fits the total optslen */
                     if ((offset + 1 + ip6_optlen) > optslen) {
                        ENGINE_SET_INVALID_EVENT(p, IPV6_EXTHDR_INVALID_OPTLEN);
                         break;
                     }
 
                    if (*ptr == IPV6OPT_PADN) /* PadN */
                    {
                        padn_cnt++;

                        /* a zero padN len would be weird */
                        if (ip6_optlen == 0)
                            ENGINE_SET_EVENT(p, IPV6_EXTHDR_ZERO_LEN_PADN);
                    }
                    else if (*ptr == IPV6OPT_RA) /* RA */
                    {
                        ra->ip6ra_type = *(ptr);
                         ra->ip6ra_len  = ip6_optlen;
 
                         if (ip6_optlen < sizeof(ra->ip6ra_value)) {
                            ENGINE_SET_INVALID_EVENT(p, IPV6_EXTHDR_INVALID_OPTLEN);
                             break;
                         }
 
                        memcpy(&ra->ip6ra_value, (ptr + 2), sizeof(ra->ip6ra_value));
                        ra->ip6ra_value = SCNtohs(ra->ip6ra_value);
                        other_cnt++;
                    }
                    else if (*ptr == IPV6OPT_JUMBO) /* Jumbo */
                    {
                        jumbo->ip6j_type = *(ptr);
                         jumbo->ip6j_len  = ip6_optlen;
 
                         if (ip6_optlen < sizeof(jumbo->ip6j_payload_len)) {
                            ENGINE_SET_INVALID_EVENT(p, IPV6_EXTHDR_INVALID_OPTLEN);
                             break;
                         }
 
                        memcpy(&jumbo->ip6j_payload_len, (ptr+2), sizeof(jumbo->ip6j_payload_len));
                        jumbo->ip6j_payload_len = SCNtohl(jumbo->ip6j_payload_len);
                    }
                    else if (*ptr == IPV6OPT_HAO) /* HAO */
                    {
                        hao->ip6hao_type = *(ptr);
                         hao->ip6hao_len  = ip6_optlen;
 
                         if (ip6_optlen < sizeof(hao->ip6hao_hoa)) {
                            ENGINE_SET_INVALID_EVENT(p, IPV6_EXTHDR_INVALID_OPTLEN);
                             break;
                         }
 
                        memcpy(&hao->ip6hao_hoa, (ptr+2), sizeof(hao->ip6hao_hoa));
                        other_cnt++;
                    } else {
                        if (nh == IPPROTO_HOPOPTS)
                            ENGINE_SET_EVENT(p, IPV6_HOPOPTS_UNKNOWN_OPT);
                        else
                            ENGINE_SET_EVENT(p, IPV6_DSTOPTS_UNKNOWN_OPT);

                        other_cnt++;
                    }
                    uint16_t optlen = (*(ptr + 1) + 2);
                    ptr += optlen; /* +2 for opt type and opt len fields */
                    offset += optlen;
                }
                /* flag packets that have only padding */
                if (padn_cnt > 0 && other_cnt == 0) {
                    if (nh == IPPROTO_HOPOPTS)
                        ENGINE_SET_EVENT(p, IPV6_HOPOPTS_ONLY_PADDING);
                    else
                        ENGINE_SET_EVENT(p, IPV6_DSTOPTS_ONLY_PADDING);
                }

                nh = *pkt;
                pkt += hdrextlen;
                plen -= hdrextlen;
                break;
            }

            case IPPROTO_FRAGMENT:
            {
                IPV6_SET_L4PROTO(p,nh);
                /* store the offset of this extension into the packet
                 * past the ipv6 header. We use it in defrag for creating
                 * a defragmented packet without the frag header */
                if (exthdr_fh_done == 0) {
                    p->ip6eh.fh_offset = pkt - orig_pkt;
                    exthdr_fh_done = 1;
                }

                 uint16_t prev_hdrextlen = hdrextlen;
                 hdrextlen = sizeof(IPV6FragHdr);
                 if (hdrextlen > plen) {
                    ENGINE_SET_INVALID_EVENT(p, IPV6_TRUNC_EXTHDR);
                     SCReturn;
                 }
 
                /* for the frag header, the length field is reserved */
                if (*(pkt + 1) != 0) {
                    ENGINE_SET_EVENT(p, IPV6_FH_NON_ZERO_RES_FIELD);
                    /* non fatal, lets try to continue */
                }

                if (IPV6_EXTHDR_ISSET_FH(p)) {
                    ENGINE_SET_EVENT(p, IPV6_EXTHDR_DUPL_FH);
                    nh = *pkt;
                    pkt += hdrextlen;
                    plen -= hdrextlen;
                    break;
                }

                /* set the header flag first */
                IPV6_EXTHDR_SET_FH(p);

                /* parse the header and setup the vars */
                DecodeIPV6FragHeader(p, pkt, hdrextlen, plen, prev_hdrextlen);

                /* if FH has offset 0 and no more fragments are coming, we
                 * parse this packet further right away, no defrag will be
                 * needed. It is a useless FH then though, so we do set an
                 * decoder event. */
                if (p->ip6eh.fh_more_frags_set == 0 && p->ip6eh.fh_offset == 0) {
                    ENGINE_SET_EVENT(p, IPV6_EXTHDR_USELESS_FH);

                    nh = *pkt;
                    pkt += hdrextlen;
                    plen -= hdrextlen;
                    break;
                }

                /* the rest is parsed upon reassembly */
                p->flags |= PKT_IS_FRAGMENT;
                SCReturn;
            }
            case IPPROTO_ESP:
            {
                 IPV6_SET_L4PROTO(p,nh);
                 hdrextlen = sizeof(IPV6EspHdr);
                 if (hdrextlen > plen) {
                    ENGINE_SET_INVALID_EVENT(p, IPV6_TRUNC_EXTHDR);
                     SCReturn;
                 }
 
                if (eh) {
                    ENGINE_SET_EVENT(p, IPV6_EXTHDR_DUPL_EH);
                    SCReturn;
                }

                eh = 1;

                nh = IPPROTO_NONE;
                pkt += hdrextlen;
                plen -= hdrextlen;
                break;
            }
            case IPPROTO_AH:
            {
                IPV6_SET_L4PROTO(p,nh);
                /* we need the header as a minimum */
                hdrextlen = sizeof(IPV6AuthHdr);
                /* the payload len field is the number of extra 4 byte fields,
                 * IPV6AuthHdr already contains the first */
                if (*(pkt+1) > 0)
                    hdrextlen += ((*(pkt+1) - 1) * 4);

                 SCLogDebug("hdrextlen %"PRIu8, hdrextlen);
 
                 if (hdrextlen > plen) {
                    ENGINE_SET_INVALID_EVENT(p, IPV6_TRUNC_EXTHDR);
                     SCReturn;
                 }
 
                IPV6AuthHdr *ahhdr = (IPV6AuthHdr *)pkt;
                if (ahhdr->ip6ah_reserved != 0x0000) {
                    ENGINE_SET_EVENT(p, IPV6_EXTHDR_AH_RES_NOT_NULL);
                }

                if (ah) {
                    ENGINE_SET_EVENT(p, IPV6_EXTHDR_DUPL_AH);
                    nh = *pkt;
                    pkt += hdrextlen;
                    plen -= hdrextlen;
                    break;
                }

                ah = 1;

                nh = *pkt;
                pkt += hdrextlen;
                plen -= hdrextlen;
                break;
            }
            case IPPROTO_IPIP:
                IPV6_SET_L4PROTO(p,nh);
                DecodeIPv4inIPv6(tv, dtv, p, pkt, plen, pq);
                SCReturn;
            /* none, last header */
            case IPPROTO_NONE:
                IPV6_SET_L4PROTO(p,nh);
                SCReturn;
            case IPPROTO_ICMP:
                ENGINE_SET_EVENT(p,IPV6_WITH_ICMPV4);
                SCReturn;
            /* no parsing yet, just skip it */
            case IPPROTO_MH:
            case IPPROTO_HIP:
             case IPPROTO_SHIM6:
                 hdrextlen = 8 + (*(pkt+1) * 8);  /* 8 bytes + length in 8 octet units */
                 if (hdrextlen > plen) {
                    ENGINE_SET_INVALID_EVENT(p, IPV6_TRUNC_EXTHDR);
                     SCReturn;
                 }
                 nh = *pkt;
                pkt += hdrextlen;
                plen -= hdrextlen;
                break;
            default:
                ENGINE_SET_EVENT(p, IPV6_UNKNOWN_NEXT_HEADER);
                IPV6_SET_L4PROTO(p,nh);
                SCReturn;
        }
    }

    SCReturn;
}
