status_t OMXNodeInstance::freeBuffer(
        OMX_U32 portIndex, OMX::buffer_id buffer) {
 Mutex::Autolock autoLock(mLock);
    CLOG_BUFFER(freeBuffer, "%s:%u %#x", portString(portIndex), portIndex, buffer);

 
     removeActiveBuffer(portIndex, buffer);
 
    OMX_BUFFERHEADERTYPE *header = findBufferHeader(buffer, portIndex);
    if (header == NULL) {
        return BAD_VALUE;
    }
     BufferMeta *buffer_meta = static_cast<BufferMeta *>(header->pAppPrivate);
 
     OMX_ERRORTYPE err = OMX_FreeBuffer(mHandle, portIndex, header);
    CLOG_IF_ERROR(freeBuffer, err, "%s:%u %#x", portString(portIndex), portIndex, buffer);

 delete buffer_meta;
    buffer_meta = NULL;
    invalidateBufferID(buffer);

 return StatusFromOMXError(err);
}
