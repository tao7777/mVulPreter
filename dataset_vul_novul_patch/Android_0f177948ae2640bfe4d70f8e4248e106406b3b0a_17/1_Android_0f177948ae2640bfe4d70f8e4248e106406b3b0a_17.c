 status_t OMXNodeInstance::updateNativeHandleInMeta(
        OMX_U32 portIndex, const sp<NativeHandle>& nativeHandle, OMX::buffer_id buffer) {
 Mutex::Autolock autoLock(mLock);
    OMX_BUFFERHEADERTYPE *header = findBufferHeader(buffer, portIndex);
 if (header == NULL) {
        ALOGE("b/25884056");
 return BAD_VALUE;
 }

 if (portIndex != kPortIndexInput && portIndex != kPortIndexOutput) {
 return BAD_VALUE;
 }


     BufferMeta *bufferMeta = (BufferMeta *)(header->pAppPrivate);
     sp<ABuffer> data = bufferMeta->getBuffer(
            header, portIndex == kPortIndexInput /* backup */, false /* limit */);
     bufferMeta->setNativeHandle(nativeHandle);
     if (mMetadataType[portIndex] == kMetadataBufferTypeNativeHandleSource
             && data->capacity() >= sizeof(VideoNativeHandleMetadata)) {
 VideoNativeHandleMetadata &metadata = *(VideoNativeHandleMetadata *)(data->data());
        metadata.eType = mMetadataType[portIndex];
        metadata.pHandle =
            nativeHandle == NULL ? NULL : const_cast<native_handle*>(nativeHandle->handle());
 } else {
        CLOG_ERROR(updateNativeHandleInMeta, BAD_VALUE, "%s:%u, %#x bad type (%d) or size (%zu)",
            portString(portIndex), portIndex, buffer, mMetadataType[portIndex], data->capacity());
 return BAD_VALUE;
 }

    CLOG_BUFFER(updateNativeHandleInMeta, "%s:%u, %#x := %p",
            portString(portIndex), portIndex, buffer,
            nativeHandle == NULL ? NULL : nativeHandle->handle());
 return OK;
}
