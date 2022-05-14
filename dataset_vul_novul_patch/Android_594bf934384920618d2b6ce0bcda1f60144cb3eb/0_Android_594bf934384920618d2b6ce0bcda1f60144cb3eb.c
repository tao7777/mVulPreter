void SoftAACEncoder2::onQueueFilled(OMX_U32 /* portIndex */) {
 if (mSignalledError) {
 return;
 }

 List<BufferInfo *> &inQueue = getPortQueue(0);
 List<BufferInfo *> &outQueue = getPortQueue(1);

 if (!mSentCodecSpecificData) {

 if (outQueue.empty()) {
 return;
 }

 if (AACENC_OK != aacEncEncode(mAACEncoder, NULL, NULL, NULL, NULL)) {
            ALOGE("Unable to initialize encoder for profile / sample-rate / bit-rate / channels");
            notify(OMX_EventError, OMX_ErrorUndefined, 0, NULL);
            mSignalledError = true;
 return;
 }

        OMX_U32 actualBitRate  = aacEncoder_GetParam(mAACEncoder, AACENC_BITRATE);
 if (mBitRate != actualBitRate) {
            ALOGW("Requested bitrate %u unsupported, using %u", mBitRate, actualBitRate);
 }

        AACENC_InfoStruct encInfo;
 if (AACENC_OK != aacEncInfo(mAACEncoder, &encInfo)) {
            ALOGE("Failed to get AAC encoder info");
            notify(OMX_EventError, OMX_ErrorUndefined, 0, NULL);
            mSignalledError = true;
 return;
 }

 
         BufferInfo *outInfo = *outQueue.begin();
         OMX_BUFFERHEADERTYPE *outHeader = outInfo->mHeader;

        if (outHeader->nOffset + encInfo.confSize > outHeader->nAllocLen) {
            ALOGE("b/34617444");
            android_errorWriteLog(0x534e4554,"34617444");
            notify(OMX_EventError, OMX_ErrorUndefined, 0, NULL);
            mSignalledError = true;
            return;
        }

         outHeader->nFilledLen = encInfo.confSize;
         outHeader->nFlags = OMX_BUFFERFLAG_CODECCONFIG;
 
 uint8_t *out = outHeader->pBuffer + outHeader->nOffset;
        memcpy(out, encInfo.confBuf, encInfo.confSize);

        outQueue.erase(outQueue.begin());
        outInfo->mOwnedByUs = false;
        notifyFillBufferDone(outHeader);

        mSentCodecSpecificData = true;
 }

 size_t numBytesPerInputFrame =
        mNumChannels * kNumSamplesPerFrame * sizeof(int16_t);

 if (mAACProfile == OMX_AUDIO_AACObjectELD && numBytesPerInputFrame > 512) {
        numBytesPerInputFrame = 512;
 }

 for (;;) {

 while (mInputSize < numBytesPerInputFrame) {

 if (mSawInputEOS || inQueue.empty()) {
 return;
 }

 BufferInfo *inInfo = *inQueue.begin();
            OMX_BUFFERHEADERTYPE *inHeader = inInfo->mHeader;

 const void *inData = inHeader->pBuffer + inHeader->nOffset;

 size_t copy = numBytesPerInputFrame - mInputSize;
 if (copy > inHeader->nFilledLen) {
                copy = inHeader->nFilledLen;
 }

 if (mInputFrame == NULL) {
                mInputFrame = new int16_t[numBytesPerInputFrame / sizeof(int16_t)];
 }

 if (mInputSize == 0) {
                mInputTimeUs = inHeader->nTimeStamp;
 }

            memcpy((uint8_t *)mInputFrame + mInputSize, inData, copy);
            mInputSize += copy;

            inHeader->nOffset += copy;
            inHeader->nFilledLen -= copy;

            inHeader->nTimeStamp +=
 (copy * 1000000ll / mSampleRate)
 / (mNumChannels * sizeof(int16_t));

 if (inHeader->nFilledLen == 0) {
 if (inHeader->nFlags & OMX_BUFFERFLAG_EOS) {
                    mSawInputEOS = true;

                    memset((uint8_t *)mInputFrame + mInputSize,
 0,
                           numBytesPerInputFrame - mInputSize);

                    mInputSize = numBytesPerInputFrame;
 }

                inQueue.erase(inQueue.begin());
                inInfo->mOwnedByUs = false;
                notifyEmptyBufferDone(inHeader);

                inData = NULL;
                inHeader = NULL;
                inInfo = NULL;
 }
 }


 if (outQueue.empty()) {
 return;
 }

 BufferInfo *outInfo = *outQueue.begin();
        OMX_BUFFERHEADERTYPE *outHeader = outInfo->mHeader;

 uint8_t *outPtr = (uint8_t *)outHeader->pBuffer + outHeader->nOffset;
 size_t outAvailable = outHeader->nAllocLen - outHeader->nOffset;

        AACENC_InArgs inargs;
        AACENC_OutArgs outargs;
        memset(&inargs, 0, sizeof(inargs));
        memset(&outargs, 0, sizeof(outargs));
        inargs.numInSamples = numBytesPerInputFrame / sizeof(int16_t);

 void* inBuffer[] = { (unsigned char *)mInputFrame };
        INT   inBufferIds[] = { IN_AUDIO_DATA };
        INT   inBufferSize[] = { (INT)numBytesPerInputFrame };
        INT   inBufferElSize[] = { sizeof(int16_t) };

        AACENC_BufDesc inBufDesc;
        inBufDesc.numBufs           = sizeof(inBuffer) / sizeof(void*);
        inBufDesc.bufs              = (void**)&inBuffer;
        inBufDesc.bufferIdentifiers = inBufferIds;
        inBufDesc.bufSizes          = inBufferSize;
        inBufDesc.bufElSizes        = inBufferElSize;

 void* outBuffer[] = { outPtr };
        INT   outBufferIds[] = { OUT_BITSTREAM_DATA };
        INT   outBufferSize[] = { 0 };
        INT   outBufferElSize[] = { sizeof(UCHAR) };

        AACENC_BufDesc outBufDesc;
        outBufDesc.numBufs           = sizeof(outBuffer) / sizeof(void*);
        outBufDesc.bufs              = (void**)&outBuffer;
        outBufDesc.bufferIdentifiers = outBufferIds;
        outBufDesc.bufSizes          = outBufferSize;
        outBufDesc.bufElSizes        = outBufferElSize;

        AACENC_ERROR encoderErr = AACENC_OK;
 size_t nOutputBytes = 0;

 do {
            memset(&outargs, 0, sizeof(outargs));

            outBuffer[0] = outPtr;
            outBufferSize[0] = outAvailable - nOutputBytes;

            encoderErr = aacEncEncode(mAACEncoder,
 &inBufDesc,
 &outBufDesc,
 &inargs,
 &outargs);

 if (encoderErr == AACENC_OK) {
                outPtr += outargs.numOutBytes;
                nOutputBytes += outargs.numOutBytes;

 if (outargs.numInSamples > 0) {
 int numRemainingSamples = inargs.numInSamples - outargs.numInSamples;
 if (numRemainingSamples > 0) {
                        memmove(mInputFrame,
 &mInputFrame[outargs.numInSamples],
 sizeof(int16_t) * numRemainingSamples);
 }
                    inargs.numInSamples -= outargs.numInSamples;
 }
 }
 } while (encoderErr == AACENC_OK && inargs.numInSamples > 0);

        outHeader->nFilledLen = nOutputBytes;

        outHeader->nFlags = OMX_BUFFERFLAG_ENDOFFRAME;

 if (mSawInputEOS) {
            outHeader->nFlags = OMX_BUFFERFLAG_EOS;
 }

        outHeader->nTimeStamp = mInputTimeUs;

#if 0
        ALOGI("sending %d bytes of data (time = %lld us, flags = 0x%08lx)",
              nOutputBytes, mInputTimeUs, outHeader->nFlags);

        hexdump(outHeader->pBuffer + outHeader->nOffset, outHeader->nFilledLen);
#endif

        outQueue.erase(outQueue.begin());
        outInfo->mOwnedByUs = false;
        notifyFillBufferDone(outHeader);

        outHeader = NULL;
        outInfo = NULL;

        mInputSize = 0;
 }
}
