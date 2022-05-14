void SoftMP3::onQueueFilled(OMX_U32 /* portIndex */) {
 if (mSignalledError || mOutputPortSettingsChange != NONE) {
 return;
 }

 List<BufferInfo *> &inQueue = getPortQueue(0);
 List<BufferInfo *> &outQueue = getPortQueue(1);

 while ((!inQueue.empty() || (mSawInputEos && !mSignalledOutputEos)) && !outQueue.empty()) {
 BufferInfo *inInfo = NULL;
        OMX_BUFFERHEADERTYPE *inHeader = NULL;
 if (!inQueue.empty()) {
            inInfo = *inQueue.begin();
            inHeader = inInfo->mHeader;
 }

 BufferInfo *outInfo = *outQueue.begin();
        OMX_BUFFERHEADERTYPE *outHeader = outInfo->mHeader;
        outHeader->nFlags = 0;

 if (inHeader) {
 if (inHeader->nOffset == 0 && inHeader->nFilledLen) {
                mAnchorTimeUs = inHeader->nTimeStamp;
                mNumFramesOutput = 0;
 }

 if (inHeader->nFlags & OMX_BUFFERFLAG_EOS) {
                mSawInputEos = true;
 }

            mConfig->pInputBuffer =
                inHeader->pBuffer + inHeader->nOffset;

            mConfig->inputBufferCurrentLength = inHeader->nFilledLen;
 } else {
            mConfig->pInputBuffer = NULL;
            mConfig->inputBufferCurrentLength = 0;
 }
        mConfig->inputBufferMaxLength = 0;

         mConfig->inputBufferUsedLength = 0;
 
         mConfig->outputFrameSize = kOutputBufferSize / sizeof(int16_t);
 
         mConfig->pOutputBuffer =
             reinterpret_cast<int16_t *>(outHeader->pBuffer);

        ERROR_CODE decoderErr;
 if ((decoderErr = pvmp3_framedecoder(mConfig, mDecoderBuf))
 != NO_DECODING_ERROR) {
            ALOGV("mp3 decoder returned error %d", decoderErr);

 if (decoderErr != NO_ENOUGH_MAIN_DATA_ERROR
 && decoderErr != SIDE_INFO_ERROR) {
                ALOGE("mp3 decoder returned error %d", decoderErr);

                notify(OMX_EventError, OMX_ErrorUndefined, decoderErr, NULL);
                mSignalledError = true;
 return;
 }

 if (mConfig->outputFrameSize == 0) {
                mConfig->outputFrameSize = kOutputBufferSize / sizeof(int16_t);
 }

 if (decoderErr == NO_ENOUGH_MAIN_DATA_ERROR && mSawInputEos) {
 if (!mIsFirst) {
                    outHeader->nOffset = 0;
                    outHeader->nFilledLen = kPVMP3DecoderDelay * mNumChannels * sizeof(int16_t);

                    memset(outHeader->pBuffer, 0, outHeader->nFilledLen);
 }
                outHeader->nFlags = OMX_BUFFERFLAG_EOS;
                mSignalledOutputEos = true;
 } else {

                ALOGV_IF(mIsFirst, "insufficient data for first frame, sending silence");
                memset(outHeader->pBuffer,
 0,
                       mConfig->outputFrameSize * sizeof(int16_t));

 if (inHeader) {
                    mConfig->inputBufferUsedLength = inHeader->nFilledLen;
 }
 }
 } else if (mConfig->samplingRate != mSamplingRate
 || mConfig->num_channels != mNumChannels) {
            mSamplingRate = mConfig->samplingRate;
            mNumChannels = mConfig->num_channels;

            notify(OMX_EventPortSettingsChanged, 1, 0, NULL);
            mOutputPortSettingsChange = AWAITING_DISABLED;
 return;
 }

 if (mIsFirst) {
            mIsFirst = false;
            outHeader->nOffset =
                kPVMP3DecoderDelay * mNumChannels * sizeof(int16_t);

            outHeader->nFilledLen =
                mConfig->outputFrameSize * sizeof(int16_t) - outHeader->nOffset;
 } else if (!mSignalledOutputEos) {
            outHeader->nOffset = 0;
            outHeader->nFilledLen = mConfig->outputFrameSize * sizeof(int16_t);
 }

        outHeader->nTimeStamp =
            mAnchorTimeUs + (mNumFramesOutput * 1000000ll) / mSamplingRate;

 if (inHeader) {
            CHECK_GE(inHeader->nFilledLen, mConfig->inputBufferUsedLength);

            inHeader->nOffset += mConfig->inputBufferUsedLength;
            inHeader->nFilledLen -= mConfig->inputBufferUsedLength;


 if (inHeader->nFilledLen == 0) {
                inInfo->mOwnedByUs = false;
                inQueue.erase(inQueue.begin());
                inInfo = NULL;
                notifyEmptyBufferDone(inHeader);
                inHeader = NULL;
 }
 }

        mNumFramesOutput += mConfig->outputFrameSize / mNumChannels;

        outInfo->mOwnedByUs = false;
        outQueue.erase(outQueue.begin());
        outInfo = NULL;
        notifyFillBufferDone(outHeader);
        outHeader = NULL;
 }
}
