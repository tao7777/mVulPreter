OMX_BUFFERHEADERTYPE *OMXNodeInstance::findBufferHeader(OMX::buffer_id buffer) {
OMX_BUFFERHEADERTYPE *OMXNodeInstance::findBufferHeader(
        OMX::buffer_id buffer, OMX_U32 portIndex) {
     if (buffer == 0) {
         return NULL;
     }
 Mutex::Autolock autoLock(mBufferIDLock);
 ssize_t index = mBufferIDToBufferHeader.indexOfKey(buffer);
 if (index < 0) {

         CLOGW("findBufferHeader: buffer %u not found", buffer);
         return NULL;
     }
    OMX_BUFFERHEADERTYPE *header = mBufferIDToBufferHeader.valueAt(index);
    BufferMeta *buffer_meta =
        static_cast<BufferMeta *>(header->pAppPrivate);
    if (buffer_meta->getPortIndex() != portIndex) {
        CLOGW("findBufferHeader: buffer %u found but with incorrect port index.", buffer);
        android_errorWriteLog(0x534e4554, "28816827");
        return NULL;
    }
    return header;
 }
