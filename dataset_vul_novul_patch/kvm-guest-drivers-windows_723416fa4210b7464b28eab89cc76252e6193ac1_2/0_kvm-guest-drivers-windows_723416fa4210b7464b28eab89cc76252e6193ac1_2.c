void CNB::SetupLSO(virtio_net_hdr_basic *VirtioHeader, PVOID IpHeader, ULONG EthPayloadLength) const
{
    PopulateIPLength(reinterpret_cast<IPv4Header*>(IpHeader), static_cast<USHORT>(EthPayloadLength));

     tTcpIpPacketParsingResult packetReview;
     packetReview = ParaNdis_CheckSumVerifyFlat(reinterpret_cast<IPv4Header*>(IpHeader), EthPayloadLength,
                                                pcrIpChecksum | pcrFixIPChecksum | pcrTcpChecksum | pcrFixPHChecksum,
                                               FALSE,
                                                __FUNCTION__);
 
     if (packetReview.xxpCheckSum == ppresPCSOK || packetReview.fixedXxpCS)
    {
        auto IpHeaderOffset = m_Context->Offload.ipHeaderOffset;
        auto VHeader = static_cast<virtio_net_hdr_basic*>(VirtioHeader);
        auto PriorityHdrLen = (m_ParentNBL->TCI() != 0) ? ETH_PRIORITY_HEADER_SIZE : 0;

        VHeader->flags = VIRTIO_NET_HDR_F_NEEDS_CSUM;
        VHeader->gso_type = packetReview.ipStatus == ppresIPV4 ? VIRTIO_NET_HDR_GSO_TCPV4 : VIRTIO_NET_HDR_GSO_TCPV6;
        VHeader->hdr_len = (USHORT)(packetReview.XxpIpHeaderSize + IpHeaderOffset + PriorityHdrLen);
        VHeader->gso_size = (USHORT)m_ParentNBL->MSS();
        VHeader->csum_start = (USHORT)(m_ParentNBL->TCPHeaderOffset() + PriorityHdrLen);
        VHeader->csum_offset = TCP_CHECKSUM_OFFSET;
    }
}
