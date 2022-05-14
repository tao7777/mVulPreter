void SoftAMR::onQueueFilled(OMX_U32 /* portIndex */) {
 List<BufferInfo *> &inQueue = getPortQueue(0);
 List<BufferInfo *> &outQueue = getPortQueue(1);

 if (mSignalledError || mOutputPortSettingsChange != NONE) {
 return;
 }

 while (!inQueue.empty() && !outQueue.empty()) {

         BufferInfo *inInfo = *inQueue.begin();
         OMX_BUFFERHEADERTYPE *inHeader = inInfo->mHeader;
 
        if (inHeader->nFilledLen == 0) {
            inInfo->mOwnedByUs = false;
            inQueue.erase(inQueue.begin());
            notifyEmptyBufferDone(inHeader);
            continue;
        }

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

 if (inHeader->nOffset == 0) {
            mAnchorTimeUs = inHeader->nTimeStamp;
            mNumSamplesOutput = 0;
 }

 const uint8_t *inputPtr = inHeader->pBuffer + inHeader->nOffset;
 int32_t numBytesRead;

 if (mMode == MODE_NARROW) {
 if (outHeader->nAllocLen < kNumSamplesPerFrameNB * sizeof(int16_t)) {
                ALOGE("b/27662364: NB expected output buffer %zu bytes vs %u",
                       kNumSamplesPerFrameNB * sizeof(int16_t), outHeader->nAllocLen);
                android_errorWriteLog(0x534e4554, "27662364");
                notify(OMX_EventError, OMX_ErrorOverflow, 0, NULL);
                mSignalledError = true;

                 return;
             }
 
            int16 mode = ((inputPtr[0] >> 3) & 0x0f);
            // for WMF since MIME_IETF is used when calling AMRDecode.
            size_t frameSize = WmfDecBytesPerFrame[mode] + 1;

            if (inHeader->nFilledLen < frameSize) {
                ALOGE("b/27662364: expected %zu bytes vs %u", frameSize, inHeader->nFilledLen);
                notify(OMX_EventError, OMX_ErrorStreamCorrupt, 0, NULL);
                mSignalledError = true;
                return;
            }

             numBytesRead =
                 AMRDecode(mState,
                   (Frame_Type_3GPP)((inputPtr[0] >> 3) & 0x0f),
 (UWord8 *)&inputPtr[1],
 reinterpret_cast<int16_t *>(outHeader->pBuffer),
                  MIME_IETF);

 if (numBytesRead == -1) {
                ALOGE("PV AMR decoder AMRDecode() call failed");

                notify(OMX_EventError, OMX_ErrorUndefined, 0, NULL);
                mSignalledError = true;

 return;
 }

 ++numBytesRead; // Include the frame type header byte.

 if (static_cast<size_t>(numBytesRead) > inHeader->nFilledLen) {

                notify(OMX_EventError, OMX_ErrorUndefined, 0, NULL);
                mSignalledError = true;

 return;
 }
 } else {
 if (outHeader->nAllocLen < kNumSamplesPerFrameWB * sizeof(int16_t)) {
                ALOGE("b/27662364: WB expected output buffer %zu bytes vs %u",
                       kNumSamplesPerFrameWB * sizeof(int16_t), outHeader->nAllocLen);
                android_errorWriteLog(0x534e4554, "27662364");
                notify(OMX_EventError, OMX_ErrorOverflow, 0, NULL);
                mSignalledError = true;
 return;
 }

 int16 mode = ((inputPtr[0] >> 3) & 0x0f);

 if (mode >= 10 && mode <= 13) {
                ALOGE("encountered illegal frame type %d in AMR WB content.",
                      mode);

                notify(OMX_EventError, OMX_ErrorUndefined, 0, NULL);
                mSignalledError = true;

 return;

             }
 
             size_t frameSize = getFrameSize(mode);
            if (inHeader->nFilledLen < frameSize) {
                ALOGE("b/27662364: expected %zu bytes vs %u", frameSize, inHeader->nFilledLen);
                notify(OMX_EventError, OMX_ErrorStreamCorrupt, 0, NULL);
                mSignalledError = true;
                return;
            }
 
             int16_t *outPtr = (int16_t *)outHeader->pBuffer;
 
 if (mode >= 9) {
                memset(outPtr, 0, kNumSamplesPerFrameWB * sizeof(int16_t));
 } else if (mode < 9) {
 int16 frameType;
                RX_State_wb rx_state;
                mime_unsorting(
 const_cast<uint8_t *>(&inputPtr[1]),
                        mInputSampleBuffer,
 &frameType, &mode, 1, &rx_state);

 int16_t numSamplesOutput;
                pvDecoder_AmrWb(
                        mode, mInputSampleBuffer,
                        outPtr,
 &numSamplesOutput,
                        mDecoderBuf, frameType, mDecoderCookie);

                CHECK_EQ((int)numSamplesOutput, (int)kNumSamplesPerFrameWB);

 for (int i = 0; i < kNumSamplesPerFrameWB; ++i) {
 /* Delete the 2 LSBs (14-bit output) */
                    outPtr[i] &= 0xfffC;
 }
 }

            numBytesRead = frameSize;
 }

        inHeader->nOffset += numBytesRead;
        inHeader->nFilledLen -= numBytesRead;

        outHeader->nFlags = 0;
        outHeader->nOffset = 0;

 if (mMode == MODE_NARROW) {
            outHeader->nFilledLen = kNumSamplesPerFrameNB * sizeof(int16_t);

            outHeader->nTimeStamp =
                mAnchorTimeUs
 + (mNumSamplesOutput * 1000000ll) / kSampleRateNB;

            mNumSamplesOutput += kNumSamplesPerFrameNB;
 } else {
            outHeader->nFilledLen = kNumSamplesPerFrameWB * sizeof(int16_t);

            outHeader->nTimeStamp =
                mAnchorTimeUs
 + (mNumSamplesOutput * 1000000ll) / kSampleRateWB;

            mNumSamplesOutput += kNumSamplesPerFrameWB;
 }

 if (inHeader->nFilledLen == 0) {
            inInfo->mOwnedByUs = false;
            inQueue.erase(inQueue.begin());
            inInfo = NULL;
            notifyEmptyBufferDone(inHeader);
            inHeader = NULL;
 }

        outInfo->mOwnedByUs = false;
        outQueue.erase(outQueue.begin());
        outInfo = NULL;
        notifyFillBufferDone(outHeader);
        outHeader = NULL;

 ++mInputBufferCount;
 }
}
