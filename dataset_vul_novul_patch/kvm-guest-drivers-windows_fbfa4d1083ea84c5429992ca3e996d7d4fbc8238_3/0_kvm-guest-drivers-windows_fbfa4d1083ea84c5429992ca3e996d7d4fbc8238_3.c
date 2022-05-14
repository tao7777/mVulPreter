 QualifyIpPacket(IPHeader *pIpHeader, ULONG len)
 {
     tTcpIpPacketParsingResult res;
    res.value = 0;

    if (len < 4)
    {
        res.ipStatus = ppresNotIP;
        return res;
    }

     UCHAR  ver_len = pIpHeader->v4.ip_verlen;
     UCHAR  ip_version = (ver_len & 0xF0) >> 4;
     USHORT ipHeaderSize = 0;
     USHORT fullLength = 0;
     res.value = 0;

     if (ip_version == 4)
     {
        if (len < sizeof(IPv4Header))
        {
            res.ipStatus = ppresNotIP;
            return res;
        }
         ipHeaderSize = (ver_len & 0xF) << 2;
         fullLength = swap_short(pIpHeader->v4.ip_length);
        DPrintf(3, ("ip_version %d, ipHeaderSize %d, protocol %d, iplen %d, L2 payload length %d\n",
            ip_version, ipHeaderSize, pIpHeader->v4.ip_protocol, fullLength, len));

         res.ipStatus = (ipHeaderSize >= sizeof(IPv4Header)) ? ppresIPV4 : ppresNotIP;
        if (res.ipStatus == ppresNotIP)
         {
            return res;
        }

        if (ipHeaderSize >= fullLength || len < fullLength)
        {
            DPrintf(2, ("[%s] - truncated packet - ip_version %d, ipHeaderSize %d, protocol %d, iplen %d, L2 payload length %d\n",
                ip_version, ipHeaderSize, pIpHeader->v4.ip_protocol, fullLength, len));
            res.ipCheckSum = ppresIPTooShort;
            return res;
         }
     }
     else if (ip_version == 6)
    {
        UCHAR nextHeader = pIpHeader->v6.ip6_next_header;
        BOOLEAN bParsingDone = FALSE;
        ipHeaderSize = sizeof(pIpHeader->v6);
        res.ipStatus = ppresIPV6;
        res.ipCheckSum = ppresCSOK;
        fullLength = swap_short(pIpHeader->v6.ip6_payload_len);
        fullLength += ipHeaderSize;
        while (nextHeader != 59)
        {
            IPv6ExtHeader *pExt;
            switch (nextHeader)
            {
                case PROTOCOL_TCP:
                    bParsingDone = TRUE;
                    res.xxpStatus = ppresXxpKnown;
                    res.TcpUdp = ppresIsTCP;
                    res.xxpFull = len >= fullLength ? 1 : 0;
                    res = ProcessTCPHeader(res, pIpHeader, len, ipHeaderSize);
                    break;
                case PROTOCOL_UDP:
                    bParsingDone = TRUE;
                    res.xxpStatus = ppresXxpKnown;
                    res.TcpUdp = ppresIsUDP;
                    res.xxpFull = len >= fullLength ? 1 : 0;
                    res = ProcessUDPHeader(res, pIpHeader, len, ipHeaderSize);
                    break;
                case 0:
                case 60:
                case 43:
                case 44:
                case 51:
                case 50:
                case 135:
                    if (len >= ((ULONG)ipHeaderSize + 8))
                    {
                        pExt = (IPv6ExtHeader *)((PUCHAR)pIpHeader + ipHeaderSize);
                        nextHeader = pExt->ip6ext_next_header;
                        ipHeaderSize += 8;
                        ipHeaderSize += pExt->ip6ext_hdr_len * 8;
                    }
                    else
                    {
                        DPrintf(0, ("[%s] ERROR: Break in the middle of ext. headers(len %d, hdr > %d)\n", __FUNCTION__, len, ipHeaderSize));
                        res.ipStatus = ppresNotIP;
                        bParsingDone = TRUE;
                    }
                    break;
                default:
                    res.xxpStatus = ppresXxpOther;
                    bParsingDone = TRUE;
                    break;
            }
            if (bParsingDone)
                break;
        }
        if (ipHeaderSize <= MAX_SUPPORTED_IPV6_HEADERS)
        {
            DPrintf(3, ("ip_version %d, ipHeaderSize %d, protocol %d, iplen %d\n",
                ip_version, ipHeaderSize, nextHeader, fullLength));
            res.ipHeaderSize = ipHeaderSize;
        }
        else
        {
            DPrintf(0, ("[%s] ERROR: IP chain is too large (%d)\n", __FUNCTION__, ipHeaderSize));
            res.ipStatus = ppresNotIP;
        }
    }
    
     if (res.ipStatus == ppresIPV4)
     {
         res.ipHeaderSize = ipHeaderSize;

         res.IsFragment = (pIpHeader->v4.ip_offset & ~0xC0) != 0;
         switch (pIpHeader->v4.ip_protocol)
        {
            case PROTOCOL_TCP:
            {
                res = ProcessTCPHeader(res, pIpHeader, len, ipHeaderSize);
            }
            break;
        case PROTOCOL_UDP:
            {
                res = ProcessUDPHeader(res, pIpHeader, len, ipHeaderSize);
            }
            break;
        default:
            res.xxpStatus = ppresXxpOther;
            break;
        }
    }
    return res;
}
