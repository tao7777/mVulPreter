void SoftG711::onQueueFilled(OMX_U32 /* portIndex */) {
 if (mSignalledError) {
 return;
 }

 List<BufferInfo *> &inQueue = getPortQueue(0);
 List<BufferInfo *> &outQueue = getPortQueue(1);

 while (!inQueue.empty() && !outQueue.empty()) {
 BufferInfo *inInfo = *inQueue.begin();
        OMX_BUFFERHEADERTYPE *inHeader = inInfo->mHeader;

 BufferInfo *outInfo = *outQueue.begin();
        OMX_BUFFERHEADERTYPE *outHeader = outInfo->mHeader;

 if (inHeader->nFlags & OMX_BUFFERFLAG_EOS) {
            inQueue.erase(inQueue.begin());
            inInfo->mOwnedByUs = false;
            notifyEmptyBufferDone(inHeader);

            outHeader->nFilledLen = 0;
            outHeader->nFlags = OMX_BUFFERFLAG_EOS;

            outQueue.erase(outQueue.begin());
            outInfo->mOwnedByUs = false;
            notifyFillBufferDone(outHeader);
 return;
 }

 if (inHeader->nFilledLen > kMaxNumSamplesPerFrame) {
            ALOGE("input buffer too large (%d).", inHeader->nFilledLen);

            notify(OMX_EventError, OMX_ErrorUndefined, 0, NULL);

             mSignalledError = true;
         }
 
         const uint8_t *inputptr = inHeader->pBuffer + inHeader->nOffset;
 
         if (mIsMLaw) {
 DecodeMLaw(
 reinterpret_cast<int16_t *>(outHeader->pBuffer),
                    inputptr, inHeader->nFilledLen);
 } else {
 DecodeALaw(
 reinterpret_cast<int16_t *>(outHeader->pBuffer),
                    inputptr, inHeader->nFilledLen);
 }

        outHeader->nTimeStamp = inHeader->nTimeStamp;
        outHeader->nOffset = 0;
        outHeader->nFilledLen = inHeader->nFilledLen * sizeof(int16_t);
        outHeader->nFlags = 0;

        inInfo->mOwnedByUs = false;
        inQueue.erase(inQueue.begin());
        inInfo = NULL;
        notifyEmptyBufferDone(inHeader);
        inHeader = NULL;

        outInfo->mOwnedByUs = false;
        outQueue.erase(outQueue.begin());
        outInfo = NULL;
        notifyFillBufferDone(outHeader);
        outHeader = NULL;
 }
}
