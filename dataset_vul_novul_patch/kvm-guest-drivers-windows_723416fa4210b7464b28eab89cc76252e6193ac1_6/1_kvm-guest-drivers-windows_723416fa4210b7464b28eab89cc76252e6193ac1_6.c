tPacketIndicationType ParaNdis_PrepareReceivedPacket(
    PARANDIS_ADAPTER *pContext,
    pRxNetDescriptor pBuffersDesc,
    PUINT            pnCoalescedSegmentsCount)
{
    PMDL pMDL = pBuffersDesc->Holder;
    PNET_BUFFER_LIST pNBL = NULL;
    *pnCoalescedSegmentsCount = 1;

    if (pMDL)
    {
        ULONG nBytesStripped = 0;
        PNET_PACKET_INFO pPacketInfo = &pBuffersDesc->PacketInfo;

        if (pContext->ulPriorityVlanSetting && pPacketInfo->hasVlanHeader)
        {
            nBytesStripped = ParaNdis_StripVlanHeaderMoveHead(pPacketInfo);
        }

        ParaNdis_PadPacketToMinimalLength(pPacketInfo);
        ParaNdis_AdjustRxBufferHolderLength(pBuffersDesc, nBytesStripped);
        pNBL = NdisAllocateNetBufferAndNetBufferList(pContext->BufferListsPool, 0, 0, pMDL, nBytesStripped, pPacketInfo->dataLength);

        if (pNBL)
        {
            virtio_net_hdr_basic *pHeader = (virtio_net_hdr_basic *) pBuffersDesc->PhysicalPages[0].Virtual;
            tChecksumCheckResult csRes;
            pNBL->SourceHandle = pContext->MiniportHandle;
            NBLSetRSSInfo(pContext, pNBL, pPacketInfo);
            NBLSet8021QInfo(pContext, pNBL, pPacketInfo);

            pNBL->MiniportReserved[0] = pBuffersDesc;

#if PARANDIS_SUPPORT_RSC
            if(pHeader->gso_type != VIRTIO_NET_HDR_GSO_NONE)
            {
                *pnCoalescedSegmentsCount = PktGetTCPCoalescedSegmentsCount(pPacketInfo, pContext->MaxPacketSize.nMaxDataSize);
                NBLSetRSCInfo(pContext, pNBL, pPacketInfo, *pnCoalescedSegmentsCount);
            }
            else
#endif
            {
                csRes = ParaNdis_CheckRxChecksum(
                    pContext,
                     pHeader->flags,
                     &pBuffersDesc->PhysicalPages[PARANDIS_FIRST_RX_DATA_PAGE],
                     pPacketInfo->dataLength,
                    nBytesStripped);
                 if (csRes.value)
                 {
                     NDIS_TCP_IP_CHECKSUM_NET_BUFFER_LIST_INFO qCSInfo;
                    qCSInfo.Value = NULL;
                    qCSInfo.Receive.IpChecksumFailed = csRes.flags.IpFailed;
                    qCSInfo.Receive.IpChecksumSucceeded = csRes.flags.IpOK;
                    qCSInfo.Receive.TcpChecksumFailed = csRes.flags.TcpFailed;
                    qCSInfo.Receive.TcpChecksumSucceeded = csRes.flags.TcpOK;
                    qCSInfo.Receive.UdpChecksumFailed = csRes.flags.UdpFailed;
                    qCSInfo.Receive.UdpChecksumSucceeded = csRes.flags.UdpOK;
                    NET_BUFFER_LIST_INFO(pNBL, TcpIpChecksumNetBufferListInfo) = qCSInfo.Value;
                    DPrintf(1, ("Reporting CS %X->%X\n", csRes.value, (ULONG)(ULONG_PTR)qCSInfo.Value));
                }
            }
            pNBL->Status = NDIS_STATUS_SUCCESS;
#if defined(ENABLE_HISTORY_LOG)
            {
                tTcpIpPacketParsingResult packetReview = ParaNdis_CheckSumVerify(
                    RtlOffsetToPointer(pPacketInfo->headersBuffer, ETH_HEADER_SIZE),
                    pPacketInfo->dataLength,
                    pcrIpChecksum | pcrTcpChecksum | pcrUdpChecksum,
                    __FUNCTION__
                    );
                ParaNdis_DebugHistory(pContext, hopPacketReceived, pNBL, pPacketInfo->dataLength, (ULONG)(ULONG_PTR)qInfo.Value, packetReview.value);
            }
#endif
        }
    }
    return pNBL;
}
