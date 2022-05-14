status_t OMXNodeInstance::allocateSecureBuffer(
        OMX_U32 portIndex, size_t size, OMX::buffer_id *buffer,
 void **buffer_data, sp<NativeHandle> *native_handle) {
 if (buffer == NULL || buffer_data == NULL || native_handle == NULL) {
        ALOGE("b/25884056");

         return BAD_VALUE;
     }
 
     Mutex::Autolock autoLock(mLock);
 
     BufferMeta *buffer_meta = new BufferMeta(size, portIndex);

    OMX_BUFFERHEADERTYPE *header;

    OMX_ERRORTYPE err = OMX_AllocateBuffer(
            mHandle, &header, portIndex, buffer_meta, size);

 if (err != OMX_ErrorNone) {
        CLOG_ERROR(allocateBuffer, err, BUFFER_FMT(portIndex, "%zu@", size));
 delete buffer_meta;
        buffer_meta = NULL;

 *buffer = 0;

 return StatusFromOMXError(err);
 }

    CHECK_EQ(header->pAppPrivate, buffer_meta);

 *buffer = makeBufferID(header);
 if (mSecureBufferType[portIndex] == kSecureBufferTypeNativeHandle) {
 *buffer_data = NULL;
 *native_handle = NativeHandle::create(
 (native_handle_t *)header->pBuffer, false /* ownsHandle */);
 } else {
 *buffer_data = header->pBuffer;
 *native_handle = NULL;
 }

    addActiveBuffer(portIndex, *buffer);

    sp<GraphicBufferSource> bufferSource(getGraphicBufferSource());
 if (bufferSource != NULL && portIndex == kPortIndexInput) {
        bufferSource->addCodecBuffer(header);
 }
    CLOG_BUFFER(allocateSecureBuffer, NEW_BUFFER_FMT(
 *buffer, portIndex, "%zu@%p:%p", size, *buffer_data,
 *native_handle == NULL ? NULL : (*native_handle)->handle()));

 return OK;
}
