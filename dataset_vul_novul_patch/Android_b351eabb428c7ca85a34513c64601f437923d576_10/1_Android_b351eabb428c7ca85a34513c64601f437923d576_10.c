 status_t OMXNodeInstance::updateGraphicBufferInMeta_l(
         OMX_U32 portIndex, const sp<GraphicBuffer>& graphicBuffer,
         OMX::buffer_id buffer, OMX_BUFFERHEADERTYPE *header) {
     if (portIndex != kPortIndexInput && portIndex != kPortIndexOutput) {
         return BAD_VALUE;
     }

 BufferMeta *bufferMeta = (BufferMeta *)(header->pAppPrivate);
    bufferMeta->setGraphicBuffer(graphicBuffer);
 if (mMetadataType[portIndex] == kMetadataBufferTypeGrallocSource
 && header->nAllocLen >= sizeof(VideoGrallocMetadata)) {
 VideoGrallocMetadata &metadata = *(VideoGrallocMetadata *)(header->pBuffer);
        metadata.eType = kMetadataBufferTypeGrallocSource;
        metadata.pHandle = graphicBuffer == NULL ? NULL : graphicBuffer->handle;
 } else if (mMetadataType[portIndex] == kMetadataBufferTypeANWBuffer
 && header->nAllocLen >= sizeof(VideoNativeMetadata)) {
 VideoNativeMetadata &metadata = *(VideoNativeMetadata *)(header->pBuffer);
        metadata.eType = kMetadataBufferTypeANWBuffer;
        metadata.pBuffer = graphicBuffer == NULL ? NULL : graphicBuffer->getNativeBuffer();
        metadata.nFenceFd = -1;
 } else {
        CLOG_BUFFER(updateGraphicBufferInMeta, "%s:%u, %#x bad type (%d) or size (%u)",
            portString(portIndex), portIndex, buffer, mMetadataType[portIndex], header->nAllocLen);
 return BAD_VALUE;
 }

    CLOG_BUFFER(updateGraphicBufferInMeta, "%s:%u, %#x := %p",
            portString(portIndex), portIndex, buffer,
            graphicBuffer == NULL ? NULL : graphicBuffer->handle);
 return OK;
}
