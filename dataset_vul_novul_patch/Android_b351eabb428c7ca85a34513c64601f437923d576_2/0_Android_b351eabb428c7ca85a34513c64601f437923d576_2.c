status_t OMXNodeInstance::allocateBuffer(
        OMX_U32 portIndex, size_t size, OMX::buffer_id *buffer,

         void **buffer_data) {
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
 *buffer_data = header->pBuffer;

    addActiveBuffer(portIndex, *buffer);

    sp<GraphicBufferSource> bufferSource(getGraphicBufferSource());
 if (bufferSource != NULL && portIndex == kPortIndexInput) {
        bufferSource->addCodecBuffer(header);
 }
    CLOG_BUFFER(allocateBuffer, NEW_BUFFER_FMT(*buffer, portIndex, "%zu@%p", size, *buffer_data));

 return OK;
}
