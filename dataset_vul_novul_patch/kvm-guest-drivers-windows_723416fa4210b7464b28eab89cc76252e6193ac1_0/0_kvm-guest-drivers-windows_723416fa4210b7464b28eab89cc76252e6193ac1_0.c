void CNB::DoIPHdrCSO(PVOID IpHeader, ULONG EthPayloadLength) const
 {
     ParaNdis_CheckSumVerifyFlat(IpHeader,
                                 EthPayloadLength,
                                pcrIpChecksum | pcrFixIPChecksum, FALSE,
                                 __FUNCTION__);
 }
