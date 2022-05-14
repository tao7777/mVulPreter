tTcpIpPacketParsingResult ParaNdis_CheckSumVerify(
                                                tCompletePhysicalAddress *pDataPages,
                                                ULONG ulDataLength,
                                                ULONG ulStartOffset,
                                                ULONG flags,
                                                LPCSTR caller)
{
     IPHeader *pIpHeader = (IPHeader *) RtlOffsetToPointer(pDataPages[0].Virtual, ulStartOffset);
 
     tTcpIpPacketParsingResult res = QualifyIpPacket(pIpHeader, ulDataLength);
     if (res.ipStatus == ppresIPV4)
     {
         if (flags & pcrIpChecksum)
            res = VerifyIpChecksum(&pIpHeader->v4, res, (flags & pcrFixIPChecksum) != 0);
        if(res.xxpStatus == ppresXxpKnown)
        {
            if (res.TcpUdp == ppresIsTCP) /* TCP */
            {
                if(flags & pcrTcpV4Checksum)
                {
                    res = VerifyTcpChecksum(pDataPages, ulDataLength, ulStartOffset, res, flags & (pcrFixPHChecksum | pcrFixTcpV4Checksum));
                }
            }
            else /* UDP */
            {
                if (flags & pcrUdpV4Checksum)
                {
                    res = VerifyUdpChecksum(pDataPages, ulDataLength, ulStartOffset, res, flags & (pcrFixPHChecksum | pcrFixUdpV4Checksum));
                }
            }
        }
    }
    else if (res.ipStatus == ppresIPV6)
    {
        if(res.xxpStatus == ppresXxpKnown)
        {
            if (res.TcpUdp == ppresIsTCP) /* TCP */
            {
                if(flags & pcrTcpV6Checksum)
                {
                    res = VerifyTcpChecksum(pDataPages, ulDataLength, ulStartOffset, res, flags & (pcrFixPHChecksum | pcrFixTcpV6Checksum));
                }
            }
            else /* UDP */
            {
                if (flags & pcrUdpV6Checksum)
                {
                    res = VerifyUdpChecksum(pDataPages, ulDataLength, ulStartOffset, res, flags & (pcrFixPHChecksum | pcrFixUdpV6Checksum));
                }
            }
        }
    }
    PrintOutParsingResult(res, 1, caller);
    return res;
}
