void SoftAVC::drainOneOutputBuffer(int32_t picId, uint8_t* data) {
bool SoftAVC::drainOneOutputBuffer(int32_t picId, uint8_t* data) {
     List<BufferInfo *> &outQueue = getPortQueue(kOutputPortIndex);
     BufferInfo *outInfo = *outQueue.begin();
     OMX_BUFFERHEADERTYPE *outHeader = outInfo->mHeader;
    OMX_U32 frameSize = mWidth * mHeight * 3 / 2;
    if (outHeader->nAllocLen - outHeader->nOffset < frameSize) {
        android_errorWriteLog(0x534e4554, "27833616");
        return false;
    }
    outQueue.erase(outQueue.begin());
     OMX_BUFFERHEADERTYPE *header = mPicToHeaderMap.valueFor(picId);
     outHeader->nTimeStamp = header->nTimeStamp;
     outHeader->nFlags = header->nFlags;
    outHeader->nFilledLen = frameSize;
 
     uint8_t *dst = outHeader->pBuffer + outHeader->nOffset;
     const uint8_t *srcY = data;
 const uint8_t *srcU = srcY + mWidth * mHeight;
 const uint8_t *srcV = srcU + mWidth * mHeight / 4;
 size_t srcYStride = mWidth;
 size_t srcUStride = mWidth / 2;
 size_t srcVStride = srcUStride;
    copyYV12FrameToOutputBuffer(dst, srcY, srcU, srcV, srcYStride, srcUStride, srcVStride);

    mPicToHeaderMap.removeItem(picId);

     delete header;
     outInfo->mOwnedByUs = false;
     notifyFillBufferDone(outHeader);
    return true;
 }
