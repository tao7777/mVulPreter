void SoftMPEG4::onQueueFilled(OMX_U32 /* portIndex */) {
 if (mSignalledError || mOutputPortSettingsChange != NONE) {
 return;
 }

 List<BufferInfo *> &inQueue = getPortQueue(0);
 List<BufferInfo *> &outQueue = getPortQueue(1);

 while (!inQueue.empty() && outQueue.size() == kNumOutputBuffers) {
 BufferInfo *inInfo = *inQueue.begin();
        OMX_BUFFERHEADERTYPE *inHeader = inInfo->mHeader;
 if (inHeader == NULL) {
            inQueue.erase(inQueue.begin());
            inInfo->mOwnedByUs = false;
 continue;
 }

 PortInfo *port = editPortInfo(1);

        OMX_BUFFERHEADERTYPE *outHeader =
            port->mBuffers.editItemAt(mNumSamplesOutput & 1).mHeader;

 if (inHeader->nFilledLen == 0) {
            inQueue.erase(inQueue.begin());
            inInfo->mOwnedByUs = false;
            notifyEmptyBufferDone(inHeader);

 ++mInputBufferCount;

 if (inHeader->nFlags & OMX_BUFFERFLAG_EOS) {
                outHeader->nFilledLen = 0;
                outHeader->nFlags = OMX_BUFFERFLAG_EOS;

 List<BufferInfo *>::iterator it = outQueue.begin();
 while ((*it)->mHeader != outHeader) {
 ++it;
 }

 BufferInfo *outInfo = *it;
                outInfo->mOwnedByUs = false;
                outQueue.erase(it);
                outInfo = NULL;

                notifyFillBufferDone(outHeader);
                outHeader = NULL;
 }
 return;
 }

 uint8_t *bitstream = inHeader->pBuffer + inHeader->nOffset;
 uint32_t *start_code = (uint32_t *)bitstream;
 bool volHeader = *start_code == 0xB0010000;
 if (volHeader) {
 PVCleanUpVideoDecoder(mHandle);
            mInitialized = false;
 }

 if (!mInitialized) {
 uint8_t *vol_data[1];
 int32_t vol_size = 0;

            vol_data[0] = NULL;

 if ((inHeader->nFlags & OMX_BUFFERFLAG_CODECCONFIG) || volHeader) {
                vol_data[0] = bitstream;
                vol_size = inHeader->nFilledLen;
 }

            MP4DecodingMode mode =
 (mMode == MODE_MPEG4) ? MPEG4_MODE : H263_MODE;

 Bool success = PVInitVideoDecoder(
                    mHandle, vol_data, &vol_size, 1,
                    outputBufferWidth(), outputBufferHeight(), mode);

 if (!success) {
                ALOGW("PVInitVideoDecoder failed. Unsupported content?");

                notify(OMX_EventError, OMX_ErrorUndefined, 0, NULL);
                mSignalledError = true;
 return;
 }

            MP4DecodingMode actualMode = PVGetDecBitstreamMode(mHandle);
 if (mode != actualMode) {
                notify(OMX_EventError, OMX_ErrorUndefined, 0, NULL);
                mSignalledError = true;
 return;
 }

 PVSetPostProcType((VideoDecControls *) mHandle, 0);

 bool hasFrameData = false;
 if (inHeader->nFlags & OMX_BUFFERFLAG_CODECCONFIG) {
                inInfo->mOwnedByUs = false;
                inQueue.erase(inQueue.begin());
                inInfo = NULL;
                notifyEmptyBufferDone(inHeader);
                inHeader = NULL;
 } else if (volHeader) {
                hasFrameData = true;
 }

            mInitialized = true;

 if (mode == MPEG4_MODE && handlePortSettingsChange()) {
 return;
 }

 if (!hasFrameData) {
 continue;
 }
 }

 if (!mFramesConfigured) {
 PortInfo *port = editPortInfo(1);
            OMX_BUFFERHEADERTYPE *outHeader = port->mBuffers.editItemAt(1).mHeader;


             OMX_U32 yFrameSize = sizeof(uint8) * mHandle->size;
             if ((outHeader->nAllocLen < yFrameSize) ||
                     (outHeader->nAllocLen - yFrameSize < yFrameSize / 2)) {
                ALOGE("Too small output buffer for reference frame: %lu bytes",
                        (unsigned long)outHeader->nAllocLen);
                 android_errorWriteLog(0x534e4554, "30033990");
                 notify(OMX_EventError, OMX_ErrorUndefined, 0, NULL);
                 mSignalledError = true;
 return;
 }
 PVSetReferenceYUV(mHandle, outHeader->pBuffer);
            mFramesConfigured = true;
 }

 uint32_t useExtTimestamp = (inHeader->nOffset == 0);

 uint32_t timestamp = 0xFFFFFFFF;
 if (useExtTimestamp) {
            mPvToOmxTimeMap.add(mPvTime, inHeader->nTimeStamp);
            timestamp = mPvTime;
            mPvTime++;
 }

 int32_t bufferSize = inHeader->nFilledLen;
 int32_t tmp = bufferSize;

        OMX_U32 frameSize;
        OMX_U64 yFrameSize = (OMX_U64)mWidth * (OMX_U64)mHeight;
 if (yFrameSize > ((OMX_U64)UINT32_MAX / 3) * 2) {
            ALOGE("Frame size too large");
            notify(OMX_EventError, OMX_ErrorUndefined, 0, NULL);
            mSignalledError = true;
 return;
 }
        frameSize = (OMX_U32)(yFrameSize + (yFrameSize / 2));

 if (outHeader->nAllocLen < frameSize) {
            android_errorWriteLog(0x534e4554, "27833616");
            ALOGE("Insufficient output buffer size");
            notify(OMX_EventError, OMX_ErrorUndefined, 0, NULL);
            mSignalledError = true;
 return;
 }
 if (PVDecodeVideoFrame(
                    mHandle, &bitstream, &timestamp, &tmp,
 &useExtTimestamp,
                    outHeader->pBuffer) != PV_TRUE) {
            ALOGE("failed to decode video frame.");

            notify(OMX_EventError, OMX_ErrorUndefined, 0, NULL);
            mSignalledError = true;
 return;
 }

 if (handlePortSettingsChange()) {
 return;
 }

        outHeader->nTimeStamp = mPvToOmxTimeMap.valueFor(timestamp);
        mPvToOmxTimeMap.removeItem(timestamp);

        inHeader->nOffset += bufferSize;
        inHeader->nFilledLen = 0;
 if (inHeader->nFlags & OMX_BUFFERFLAG_EOS) {
            outHeader->nFlags = OMX_BUFFERFLAG_EOS;
 } else {
            outHeader->nFlags = 0;
 }

 if (inHeader->nFilledLen == 0) {
            inInfo->mOwnedByUs = false;
            inQueue.erase(inQueue.begin());
            inInfo = NULL;
            notifyEmptyBufferDone(inHeader);
            inHeader = NULL;
 }

 ++mInputBufferCount;

        outHeader->nOffset = 0;
        outHeader->nFilledLen = frameSize;

 List<BufferInfo *>::iterator it = outQueue.begin();
 while ((*it)->mHeader != outHeader) {
 ++it;
 }

 BufferInfo *outInfo = *it;
        outInfo->mOwnedByUs = false;
        outQueue.erase(it);
        outInfo = NULL;

        notifyFillBufferDone(outHeader);
        outHeader = NULL;

 ++mNumSamplesOutput;
 }
}
