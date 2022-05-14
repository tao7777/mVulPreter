ProcessUDPHeader(tTcpIpPacketParsingResult _res, PVOID pIpHeader, ULONG len, USHORT ipHeaderSize)
 {
     tTcpIpPacketParsingResult res = _res;
     ULONG udpDataStart = ipHeaderSize + sizeof(UDPHeader);
     res.TcpUdp = ppresIsUDP;
     res.XxpIpHeaderSize = udpDataStart;
     if (len >= udpDataStart)
     {
         UDPHeader *pUdpHeader = (UDPHeader *)RtlOffsetToPointer(pIpHeader, ipHeaderSize);
         USHORT datagramLength = swap_short(pUdpHeader->udp_length);
         res.xxpStatus = ppresXxpKnown;
        res.xxpFull = TRUE;
         DPrintf(2, ("udp: len %d, datagramLength %d\n", len, datagramLength));
     }
    else
    {
        res.xxpFull = FALSE;
        res.xxpStatus = ppresXxpIncomplete;
    }
     return res;
 }
