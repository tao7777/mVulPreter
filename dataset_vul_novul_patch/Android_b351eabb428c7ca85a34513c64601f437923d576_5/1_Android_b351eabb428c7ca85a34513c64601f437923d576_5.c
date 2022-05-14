 status_t OMXNodeInstance::fillBuffer(OMX::buffer_id buffer, int fenceFd) {
     Mutex::Autolock autoLock(mLock);
 
    OMX_BUFFERHEADERTYPE *header = findBufferHeader(buffer);
     header->nFilledLen = 0;
     header->nOffset = 0;
     header->nFlags = 0;

 status_t res = storeFenceInMeta_l(header, fenceFd, kPortIndexOutput);
 if (res != OK) {
        CLOG_ERROR(fillBuffer::storeFenceInMeta, res, EMPTY_BUFFER(buffer, header, fenceFd));
 return res;
 }

 {
 Mutex::Autolock _l(mDebugLock);
        mOutputBuffersWithCodec.add(header);
        CLOG_BUMPED_BUFFER(fillBuffer, WITH_STATS(EMPTY_BUFFER(buffer, header, fenceFd)));
 }

    OMX_ERRORTYPE err = OMX_FillThisBuffer(mHandle, header);
 if (err != OMX_ErrorNone) {
        CLOG_ERROR(fillBuffer, err, EMPTY_BUFFER(buffer, header, fenceFd));
 Mutex::Autolock _l(mDebugLock);
        mOutputBuffersWithCodec.remove(header);
 }
 return StatusFromOMXError(err);
}
