tTcpIpPacketParsingResult ParaNdis_ReviewIPPacket(PVOID buffer, ULONG size, LPCSTR caller)
tTcpIpPacketParsingResult ParaNdis_ReviewIPPacket(PVOID buffer, ULONG size, BOOLEAN verifyLength, LPCSTR caller)
 {
    tTcpIpPacketParsingResult res = QualifyIpPacket((IPHeader *) buffer, size, verifyLength);
     PrintOutParsingResult(res, 1, caller);
     return res;
 }
