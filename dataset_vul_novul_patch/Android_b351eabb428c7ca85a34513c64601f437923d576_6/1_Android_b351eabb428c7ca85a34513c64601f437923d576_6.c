OMX_BUFFERHEADERTYPE *OMXNodeInstance::findBufferHeader(OMX::buffer_id buffer) {
     if (buffer == 0) {
         return NULL;
     }
 Mutex::Autolock autoLock(mBufferIDLock);
 ssize_t index = mBufferIDToBufferHeader.indexOfKey(buffer);
 if (index < 0) {

         CLOGW("findBufferHeader: buffer %u not found", buffer);
         return NULL;
     }
    return mBufferIDToBufferHeader.valueAt(index);
 }
