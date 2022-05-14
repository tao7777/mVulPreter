ProcessTCPHeader(tTcpIpPacketParsingResult _res, PVOID pIpHeader, ULONG len, USHORT ipHeaderSize)
{
     ULONG tcpipDataAt;
     tTcpIpPacketParsingResult res = _res;
     tcpipDataAt = ipHeaderSize + sizeof(TCPHeader);
    res.xxpStatus = ppresXxpIncomplete;
     res.TcpUdp = ppresIsTCP;
 
     if (len >= tcpipDataAt)
     {
         TCPHeader *pTcpHeader = (TCPHeader *)RtlOffsetToPointer(pIpHeader, ipHeaderSize);
         res.xxpStatus = ppresXxpKnown;
         tcpipDataAt = ipHeaderSize + TCP_HEADER_LENGTH(pTcpHeader);
         res.XxpIpHeaderSize = tcpipDataAt;
     }
     else
     {
         DPrintf(2, ("tcp: %d < min headers %d\n", len, tcpipDataAt));
     }
     return res;
 }
