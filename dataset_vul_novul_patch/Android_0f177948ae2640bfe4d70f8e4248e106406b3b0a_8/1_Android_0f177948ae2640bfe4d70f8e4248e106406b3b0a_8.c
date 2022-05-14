status_t OMXNodeInstance::emptyBuffer(
        OMX::buffer_id buffer,
        OMX_U32 rangeOffset, OMX_U32 rangeLength,

         OMX_U32 flags, OMX_TICKS timestamp, int fenceFd) {
     Mutex::Autolock autoLock(mLock);
 
     OMX_BUFFERHEADERTYPE *header = findBufferHeader(buffer, kPortIndexInput);
     if (header == NULL) {
         ALOGE("b/25884056");
 return BAD_VALUE;
 }
 BufferMeta *buffer_meta =
 static_cast<BufferMeta *>(header->pAppPrivate);
    sp<ABuffer> backup = buffer_meta->getBuffer(header, true /* backup */, false /* limit */);
    sp<ABuffer> codec = buffer_meta->getBuffer(header, false /* backup */, false /* limit */);

 if (mMetadataType[kPortIndexInput] == kMetadataBufferTypeGrallocSource
 && backup->capacity() >= sizeof(VideoNativeMetadata)
 && codec->capacity() >= sizeof(VideoGrallocMetadata)
 && ((VideoNativeMetadata *)backup->base())->eType
 == kMetadataBufferTypeANWBuffer) {
 VideoNativeMetadata &backupMeta = *(VideoNativeMetadata *)backup->base();
 VideoGrallocMetadata &codecMeta = *(VideoGrallocMetadata *)codec->base();
        CLOG_BUFFER(emptyBuffer, "converting ANWB %p to handle %p",
                backupMeta.pBuffer, backupMeta.pBuffer->handle);
        codecMeta.pHandle = backupMeta.pBuffer != NULL ? backupMeta.pBuffer->handle : NULL;
        codecMeta.eType = kMetadataBufferTypeGrallocSource;
        header->nFilledLen = rangeLength ? sizeof(codecMeta) : 0;
        header->nOffset = 0;
 } else {
 if (rangeOffset > header->nAllocLen
 || rangeLength > header->nAllocLen - rangeOffset) {
            CLOG_ERROR(emptyBuffer, OMX_ErrorBadParameter, FULL_BUFFER(NULL, header, fenceFd));
 if (fenceFd >= 0) {
 ::close(fenceFd);
 }
 return BAD_VALUE;
 }
        header->nFilledLen = rangeLength;
        header->nOffset = rangeOffset;

        buffer_meta->CopyToOMX(header);
 }

 return emptyBuffer_l(header, flags, timestamp, (intptr_t)buffer, fenceFd);
}
