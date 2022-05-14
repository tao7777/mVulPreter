void SoftAAC2::onQueueFilled(OMX_U32 /* portIndex */) {
 if (mSignalledError || mOutputPortSettingsChange != NONE) {
 return;
 }

    UCHAR* inBuffer[FILEREAD_MAX_LAYERS];
    UINT inBufferLength[FILEREAD_MAX_LAYERS] = {0};
    UINT bytesValid[FILEREAD_MAX_LAYERS] = {0};

 List<BufferInfo *> &inQueue = getPortQueue(0);
 List<BufferInfo *> &outQueue = getPortQueue(1);

 while ((!inQueue.empty() || mEndOfInput) && !outQueue.empty()) {
 if (!inQueue.empty()) {
            INT_PCM tmpOutBuffer[2048 * MAX_CHANNEL_COUNT];
 BufferInfo *inInfo = *inQueue.begin();
            OMX_BUFFERHEADERTYPE *inHeader = inInfo->mHeader;

            mEndOfInput = (inHeader->nFlags & OMX_BUFFERFLAG_EOS) != 0;

 if (mInputBufferCount == 0 && !(inHeader->nFlags & OMX_BUFFERFLAG_CODECCONFIG)) {
                ALOGE("first buffer should have OMX_BUFFERFLAG_CODECCONFIG set");
                inHeader->nFlags |= OMX_BUFFERFLAG_CODECCONFIG;
 }
 if ((inHeader->nFlags & OMX_BUFFERFLAG_CODECCONFIG) != 0) {
 BufferInfo *inInfo = *inQueue.begin();
                OMX_BUFFERHEADERTYPE *inHeader = inInfo->mHeader;

                inBuffer[0] = inHeader->pBuffer + inHeader->nOffset;
                inBufferLength[0] = inHeader->nFilledLen;

                AAC_DECODER_ERROR decoderErr =
                    aacDecoder_ConfigRaw(mAACDecoder,
                                         inBuffer,
                                         inBufferLength);

 if (decoderErr != AAC_DEC_OK) {
                    ALOGW("aacDecoder_ConfigRaw decoderErr = 0x%4.4x", decoderErr);
                    mSignalledError = true;
                    notify(OMX_EventError, OMX_ErrorUndefined, decoderErr, NULL);
 return;
 }

                mInputBufferCount++;
                mOutputBufferCount++; // fake increase of outputBufferCount to keep the counters aligned

                inInfo->mOwnedByUs = false;
                inQueue.erase(inQueue.begin());
                mLastInHeader = NULL;
                inInfo = NULL;
                notifyEmptyBufferDone(inHeader);
                inHeader = NULL;

                configureDownmix();
 if (mStreamInfo->sampleRate && mStreamInfo->numChannels) {
                    ALOGI("Initially configuring decoder: %d Hz, %d channels",
                        mStreamInfo->sampleRate,
                        mStreamInfo->numChannels);

                    notify(OMX_EventPortSettingsChanged, 1, 0, NULL);
                    mOutputPortSettingsChange = AWAITING_DISABLED;
 }
 return;
 }

 if (inHeader->nFilledLen == 0) {
                inInfo->mOwnedByUs = false;
                inQueue.erase(inQueue.begin());
                mLastInHeader = NULL;
                inInfo = NULL;
                notifyEmptyBufferDone(inHeader);
                inHeader = NULL;
 continue;
 }

 if (mIsADTS) {
 size_t adtsHeaderSize = 0;

 const uint8_t *adtsHeader = inHeader->pBuffer + inHeader->nOffset;

 bool signalError = false;
 if (inHeader->nFilledLen < 7) {
                    ALOGE("Audio data too short to contain even the ADTS header. "
 "Got %d bytes.", inHeader->nFilledLen);
                    hexdump(adtsHeader, inHeader->nFilledLen);
                    signalError = true;
 } else {
 bool protectionAbsent = (adtsHeader[1] & 1);

 unsigned aac_frame_length =
 ((adtsHeader[3] & 3) << 11)
 | (adtsHeader[4] << 3)
 | (adtsHeader[5] >> 5);

 if (inHeader->nFilledLen < aac_frame_length) {
                        ALOGE("Not enough audio data for the complete frame. "
 "Got %d bytes, frame size according to the ADTS "
 "header is %u bytes.",
                                inHeader->nFilledLen, aac_frame_length);
                        hexdump(adtsHeader, inHeader->nFilledLen);

                         signalError = true;
                     } else {
                         adtsHeaderSize = (protectionAbsent ? 7 : 9);
                        if (aac_frame_length < adtsHeaderSize) {
                            signalError = true;
                        } else {
                            inBuffer[0] = (UCHAR *)adtsHeader + adtsHeaderSize;
                            inBufferLength[0] = aac_frame_length - adtsHeaderSize;
 
                            inHeader->nOffset += adtsHeaderSize;
                            inHeader->nFilledLen -= adtsHeaderSize;
                        }
                     }
                 }
 
 if (signalError) {
                    mSignalledError = true;
                    notify(OMX_EventError, OMX_ErrorStreamCorrupt, ERROR_MALFORMED, NULL);
 return;
 }

                mBufferSizes.add(inBufferLength[0]);
 if (mLastInHeader != inHeader) {
                    mBufferTimestamps.add(inHeader->nTimeStamp);
                    mLastInHeader = inHeader;
 } else {
 int64_t currentTime = mBufferTimestamps.top();
                    currentTime += mStreamInfo->aacSamplesPerFrame *
 1000000ll / mStreamInfo->aacSampleRate;
                    mBufferTimestamps.add(currentTime);
 }
 } else {
                inBuffer[0] = inHeader->pBuffer + inHeader->nOffset;
                inBufferLength[0] = inHeader->nFilledLen;
                mLastInHeader = inHeader;
                mBufferTimestamps.add(inHeader->nTimeStamp);
                mBufferSizes.add(inHeader->nFilledLen);
 }

            bytesValid[0] = inBufferLength[0];

            INT prevSampleRate = mStreamInfo->sampleRate;
            INT prevNumChannels = mStreamInfo->numChannels;

            aacDecoder_Fill(mAACDecoder,
                            inBuffer,
                            inBufferLength,
                            bytesValid);

            mDrcWrap.submitStreamData(mStreamInfo);
            mDrcWrap.update();

            UINT inBufferUsedLength = inBufferLength[0] - bytesValid[0];
            inHeader->nFilledLen -= inBufferUsedLength;
            inHeader->nOffset += inBufferUsedLength;

            AAC_DECODER_ERROR decoderErr;
 int numLoops = 0;
 do {
 if (outputDelayRingBufferSpaceLeft() <
 (mStreamInfo->frameSize * mStreamInfo->numChannels)) {
                    ALOGV("skipping decode: not enough space left in ringbuffer");
 break;
 }

 int numConsumed = mStreamInfo->numTotalBytes;
                decoderErr = aacDecoder_DecodeFrame(mAACDecoder,
                                           tmpOutBuffer,
 2048 * MAX_CHANNEL_COUNT,
 0 /* flags */);

                numConsumed = mStreamInfo->numTotalBytes - numConsumed;
                numLoops++;

 if (decoderErr == AAC_DEC_NOT_ENOUGH_BITS) {
 break;
 }
                mDecodedSizes.add(numConsumed);

 if (decoderErr != AAC_DEC_OK) {
                    ALOGW("aacDecoder_DecodeFrame decoderErr = 0x%4.4x", decoderErr);
 }

 if (bytesValid[0] != 0) {
                    ALOGE("bytesValid[0] != 0 should never happen");
                    mSignalledError = true;
                    notify(OMX_EventError, OMX_ErrorUndefined, 0, NULL);
 return;
 }

 size_t numOutBytes =
                    mStreamInfo->frameSize * sizeof(int16_t) * mStreamInfo->numChannels;

 if (decoderErr == AAC_DEC_OK) {
 if (!outputDelayRingBufferPutSamples(tmpOutBuffer,
                            mStreamInfo->frameSize * mStreamInfo->numChannels)) {
                        mSignalledError = true;
                        notify(OMX_EventError, OMX_ErrorUndefined, decoderErr, NULL);
 return;
 }
 } else {
                    ALOGW("AAC decoder returned error 0x%4.4x, substituting silence", decoderErr);

                    memset(tmpOutBuffer, 0, numOutBytes); // TODO: check for overflow

 if (!outputDelayRingBufferPutSamples(tmpOutBuffer,
                            mStreamInfo->frameSize * mStreamInfo->numChannels)) {
                        mSignalledError = true;
                        notify(OMX_EventError, OMX_ErrorUndefined, decoderErr, NULL);
 return;
 }

 if (inHeader) {
                        inHeader->nFilledLen = 0;
 }

                    aacDecoder_SetParam(mAACDecoder, AAC_TPDEC_CLEAR_BUFFER, 1);

                    mBufferSizes.pop();
 int n = 0;
 for (int i = 0; i < numLoops; i++) {
                        n += mDecodedSizes.itemAt(mDecodedSizes.size() - numLoops + i);
 }
                    mBufferSizes.add(n);

 }

 /*
                 * AAC+/eAAC+ streams can be signalled in two ways: either explicitly
                 * or implicitly, according to MPEG4 spec. AAC+/eAAC+ is a dual
                 * rate system and the sampling rate in the final output is actually
                 * doubled compared with the core AAC decoder sampling rate.
                 *
                 * Explicit signalling is done by explicitly defining SBR audio object
                 * type in the bitstream. Implicit signalling is done by embedding
                 * SBR content in AAC extension payload specific to SBR, and hence
                 * requires an AAC decoder to perform pre-checks on actual audio frames.
                 *
                 * Thus, we could not say for sure whether a stream is
                 * AAC+/eAAC+ until the first data frame is decoded.
                 */
 if (mInputBufferCount <= 2 || mOutputBufferCount > 1) { // TODO: <= 1
 if (mStreamInfo->sampleRate != prevSampleRate ||
                        mStreamInfo->numChannels != prevNumChannels) {
                        ALOGI("Reconfiguring decoder: %d->%d Hz, %d->%d channels",
                              prevSampleRate, mStreamInfo->sampleRate,
                              prevNumChannels, mStreamInfo->numChannels);

                        notify(OMX_EventPortSettingsChanged, 1, 0, NULL);
                        mOutputPortSettingsChange = AWAITING_DISABLED;

 if (inHeader && inHeader->nFilledLen == 0) {
                            inInfo->mOwnedByUs = false;
                            mInputBufferCount++;
                            inQueue.erase(inQueue.begin());
                            mLastInHeader = NULL;
                            inInfo = NULL;
                            notifyEmptyBufferDone(inHeader);
                            inHeader = NULL;
 }
 return;
 }
 } else if (!mStreamInfo->sampleRate || !mStreamInfo->numChannels) {
                    ALOGW("Invalid AAC stream");
                    mSignalledError = true;
                    notify(OMX_EventError, OMX_ErrorUndefined, decoderErr, NULL);
 return;
 }
 if (inHeader && inHeader->nFilledLen == 0) {
                    inInfo->mOwnedByUs = false;
                    mInputBufferCount++;
                    inQueue.erase(inQueue.begin());
                    mLastInHeader = NULL;
                    inInfo = NULL;
                    notifyEmptyBufferDone(inHeader);
                    inHeader = NULL;
 } else {
                    ALOGV("inHeader->nFilledLen = %d", inHeader ? inHeader->nFilledLen : 0);
 }
 } while (decoderErr == AAC_DEC_OK);
 }

 int32_t outputDelay = mStreamInfo->outputDelay * mStreamInfo->numChannels;

 if (!mEndOfInput && mOutputDelayCompensated < outputDelay) {
 int32_t toCompensate = outputDelay - mOutputDelayCompensated;
 int32_t discard = outputDelayRingBufferSamplesAvailable();
 if (discard > toCompensate) {
                discard = toCompensate;
 }
 int32_t discarded = outputDelayRingBufferGetSamples(0, discard);
            mOutputDelayCompensated += discarded;
 continue;
 }

 if (mEndOfInput) {
 while (mOutputDelayCompensated > 0) {
                INT_PCM tmpOutBuffer[2048 * MAX_CHANNEL_COUNT];
 
                 mDrcWrap.submitStreamData(mStreamInfo);
                 mDrcWrap.update();

                AAC_DECODER_ERROR decoderErr =
                    aacDecoder_DecodeFrame(mAACDecoder,
                                           tmpOutBuffer,
 2048 * MAX_CHANNEL_COUNT,
                                           AACDEC_FLUSH);
 if (decoderErr != AAC_DEC_OK) {
                    ALOGW("aacDecoder_DecodeFrame decoderErr = 0x%4.4x", decoderErr);
 }

 int32_t tmpOutBufferSamples = mStreamInfo->frameSize * mStreamInfo->numChannels;
 if (tmpOutBufferSamples > mOutputDelayCompensated) {
                    tmpOutBufferSamples = mOutputDelayCompensated;
 }
                outputDelayRingBufferPutSamples(tmpOutBuffer, tmpOutBufferSamples);
                mOutputDelayCompensated -= tmpOutBufferSamples;
 }
 }

 while (!outQueue.empty()
 && outputDelayRingBufferSamplesAvailable()
 >= mStreamInfo->frameSize * mStreamInfo->numChannels) {
 BufferInfo *outInfo = *outQueue.begin();
            OMX_BUFFERHEADERTYPE *outHeader = outInfo->mHeader;

 if (outHeader->nOffset != 0) {
                ALOGE("outHeader->nOffset != 0 is not handled");
                mSignalledError = true;
                notify(OMX_EventError, OMX_ErrorUndefined, 0, NULL);
 return;
 }

            INT_PCM *outBuffer =
 reinterpret_cast<INT_PCM *>(outHeader->pBuffer + outHeader->nOffset);
 int samplesize = mStreamInfo->numChannels * sizeof(int16_t);
 if (outHeader->nOffset
 + mStreamInfo->frameSize * samplesize
 > outHeader->nAllocLen) {
                ALOGE("buffer overflow");
                mSignalledError = true;
                notify(OMX_EventError, OMX_ErrorUndefined, 0, NULL);
 return;

 }

 int available = outputDelayRingBufferSamplesAvailable();
 int numSamples = outHeader->nAllocLen / sizeof(int16_t);
 if (numSamples > available) {
                numSamples = available;
 }
 int64_t currentTime = 0;
 if (available) {

 int numFrames = numSamples / (mStreamInfo->frameSize * mStreamInfo->numChannels);
                numSamples = numFrames * (mStreamInfo->frameSize * mStreamInfo->numChannels);

                ALOGV("%d samples available (%d), or %d frames",
                        numSamples, available, numFrames);
 int64_t *nextTimeStamp = &mBufferTimestamps.editItemAt(0);
                currentTime = *nextTimeStamp;
 int32_t *currentBufLeft = &mBufferSizes.editItemAt(0);
 for (int i = 0; i < numFrames; i++) {
 int32_t decodedSize = mDecodedSizes.itemAt(0);
                    mDecodedSizes.removeAt(0);
                    ALOGV("decoded %d of %d", decodedSize, *currentBufLeft);
 if (*currentBufLeft > decodedSize) {
 *currentBufLeft -= decodedSize;
 *nextTimeStamp += mStreamInfo->aacSamplesPerFrame *
 1000000ll / mStreamInfo->aacSampleRate;
                        ALOGV("adjusted nextTimeStamp/size to %lld/%d",
 (long long) *nextTimeStamp, *currentBufLeft);
 } else {
 if (mBufferTimestamps.size() > 0) {
                            mBufferTimestamps.removeAt(0);
                            nextTimeStamp = &mBufferTimestamps.editItemAt(0);
                            mBufferSizes.removeAt(0);
                            currentBufLeft = &mBufferSizes.editItemAt(0);
                            ALOGV("moved to next time/size: %lld/%d",
 (long long) *nextTimeStamp, *currentBufLeft);
 }
                        numFrames = i + 1;
                        numSamples = numFrames * mStreamInfo->frameSize * mStreamInfo->numChannels;
 break;
 }
 }

                ALOGV("getting %d from ringbuffer", numSamples);
 int32_t ns = outputDelayRingBufferGetSamples(outBuffer, numSamples);
 if (ns != numSamples) {
                    ALOGE("not a complete frame of samples available");
                    mSignalledError = true;
                    notify(OMX_EventError, OMX_ErrorUndefined, 0, NULL);
 return;
 }
 }

            outHeader->nFilledLen = numSamples * sizeof(int16_t);

 if (mEndOfInput && !outQueue.empty() && outputDelayRingBufferSamplesAvailable() == 0) {
                outHeader->nFlags = OMX_BUFFERFLAG_EOS;
                mEndOfOutput = true;
 } else {
                outHeader->nFlags = 0;
 }

            outHeader->nTimeStamp = currentTime;

            mOutputBufferCount++;
            outInfo->mOwnedByUs = false;
            outQueue.erase(outQueue.begin());
            outInfo = NULL;
            ALOGV("out timestamp %lld / %d", outHeader->nTimeStamp, outHeader->nFilledLen);
            notifyFillBufferDone(outHeader);
            outHeader = NULL;
 }

 if (mEndOfInput) {
 int ringBufAvail = outputDelayRingBufferSamplesAvailable();
 if (!outQueue.empty()
 && ringBufAvail < mStreamInfo->frameSize * mStreamInfo->numChannels) {
 if (!mEndOfOutput) {
                    mEndOfOutput = true;
 BufferInfo *outInfo = *outQueue.begin();
                    OMX_BUFFERHEADERTYPE *outHeader = outInfo->mHeader;

                    INT_PCM *outBuffer = reinterpret_cast<INT_PCM *>(outHeader->pBuffer
 + outHeader->nOffset);
 int32_t ns = outputDelayRingBufferGetSamples(outBuffer, ringBufAvail);
 if (ns < 0) {
                        ns = 0;
 }
                    outHeader->nFilledLen = ns;
                    outHeader->nFlags = OMX_BUFFERFLAG_EOS;

                    outHeader->nTimeStamp = mBufferTimestamps.itemAt(0);
                    mBufferTimestamps.clear();
                    mBufferSizes.clear();
                    mDecodedSizes.clear();

                    mOutputBufferCount++;
                    outInfo->mOwnedByUs = false;
                    outQueue.erase(outQueue.begin());
                    outInfo = NULL;
                    notifyFillBufferDone(outHeader);
                    outHeader = NULL;
 }
 break; // if outQueue not empty but no more output
 }
 }
 }
}
