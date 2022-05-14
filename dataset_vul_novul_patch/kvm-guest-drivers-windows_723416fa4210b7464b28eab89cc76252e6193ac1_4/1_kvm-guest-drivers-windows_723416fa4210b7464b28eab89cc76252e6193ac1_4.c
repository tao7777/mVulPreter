tTcpIpPacketParsingResult ParaNdis_ReviewIPPacket(PVOID buffer, ULONG size, LPCSTR caller)
 {
    tTcpIpPacketParsingResult res = QualifyIpPacket((IPHeader *) buffer, size);
     PrintOutParsingResult(res, 1, caller);
     return res;
 }
