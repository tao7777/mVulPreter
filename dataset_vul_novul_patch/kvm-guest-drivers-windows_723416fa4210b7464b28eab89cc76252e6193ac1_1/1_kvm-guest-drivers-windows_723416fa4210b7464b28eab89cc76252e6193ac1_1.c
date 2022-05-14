USHORT CNB::QueryL4HeaderOffset(PVOID PacketData, ULONG IpHeaderOffset) const
 {
     USHORT Res;
     auto ppr = ParaNdis_ReviewIPPacket(RtlOffsetToPointer(PacketData, IpHeaderOffset),
                                       GetDataLength(), __FUNCTION__);
     if (ppr.ipStatus != ppresNotIP)
     {
         Res = static_cast<USHORT>(IpHeaderOffset + ppr.ipHeaderSize);
    }
    else
    {
        DPrintf(0, ("[%s] ERROR: NOT an IP packet - expected troubles!\n", __FUNCTION__));
        Res = 0;
    }
    return Res;
}
