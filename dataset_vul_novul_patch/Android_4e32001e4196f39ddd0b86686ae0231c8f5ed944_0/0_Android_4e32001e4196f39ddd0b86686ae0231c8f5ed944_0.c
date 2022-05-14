void SoftVorbis::onQueueFilled(OMX_U32 portIndex) {
 List<BufferInfo *> &inQueue = getPortQueue(0);
 List<BufferInfo *> &outQueue = getPortQueue(1);

 if (mOutputPortSettingsChange != NONE) {
 return;
 }

 if (portIndex == 0 && mInputBufferCount < 2) {
 BufferInfo *info = *inQueue.begin();
        OMX_BUFFERHEADERTYPE *header = info->mHeader;

 
         const uint8_t *data = header->pBuffer + header->nOffset;
         size_t size = header->nFilledLen;
        if (size < 7) {
            ALOGE("Too small input buffer: %zu bytes", size);
            android_errorWriteLog(0x534e4554, "27833616");
            notify(OMX_EventError, OMX_ErrorUndefined, 0, NULL);
            return;
        }
 
         ogg_buffer buf;
         ogg_reference ref;
        oggpack_buffer bits;

        makeBitReader(
 (const uint8_t *)data + 7, size - 7,
 &buf, &ref, &bits);

 if (mInputBufferCount == 0) {
            CHECK(mVi == NULL);
            mVi = new vorbis_info;
            vorbis_info_init(mVi);

            CHECK_EQ(0, _vorbis_unpack_info(mVi, &bits));
 } else {
            CHECK_EQ(0, _vorbis_unpack_books(mVi, &bits));

            CHECK(mState == NULL);
            mState = new vorbis_dsp_state;
            CHECK_EQ(0, vorbis_dsp_init(mState, mVi));

            notify(OMX_EventPortSettingsChanged, 1, 0, NULL);
            mOutputPortSettingsChange = AWAITING_DISABLED;
 }

        inQueue.erase(inQueue.begin());
        info->mOwnedByUs = false;
        notifyEmptyBufferDone(header);

 ++mInputBufferCount;

 return;
 }

 while ((!inQueue.empty() || (mSawInputEos && !mSignalledOutputEos)) && !outQueue.empty()) {
 BufferInfo *inInfo = NULL;
        OMX_BUFFERHEADERTYPE *inHeader = NULL;
 if (!inQueue.empty()) {
            inInfo = *inQueue.begin();
            inHeader = inInfo->mHeader;
 }

 BufferInfo *outInfo = *outQueue.begin();
        OMX_BUFFERHEADERTYPE *outHeader = outInfo->mHeader;

 int32_t numPageSamples = 0;

 if (inHeader) {
 if (inHeader->nFlags & OMX_BUFFERFLAG_EOS) {
                mSawInputEos = true;
 }

 if (inHeader->nFilledLen || !mSawInputEos) {
                CHECK_GE(inHeader->nFilledLen, sizeof(numPageSamples));
                memcpy(&numPageSamples,
                       inHeader->pBuffer
 + inHeader->nOffset + inHeader->nFilledLen - 4,
 sizeof(numPageSamples));

 if (inHeader->nOffset == 0) {
                    mAnchorTimeUs = inHeader->nTimeStamp;
                    mNumFramesOutput = 0;
 }

                inHeader->nFilledLen -= sizeof(numPageSamples);;
 }
 }

 if (numPageSamples >= 0) {
            mNumFramesLeftOnPage = numPageSamples;
 }

        ogg_buffer buf;
        buf.data = inHeader ? inHeader->pBuffer + inHeader->nOffset : NULL;
        buf.size = inHeader ? inHeader->nFilledLen : 0;
        buf.refcount = 1;
        buf.ptr.owner = NULL;

        ogg_reference ref;
        ref.buffer = &buf;
        ref.begin = 0;
        ref.length = buf.size;
        ref.next = NULL;

        ogg_packet pack;
        pack.packet = &ref;
        pack.bytes = ref.length;
        pack.b_o_s = 0;
        pack.e_o_s = 0;
        pack.granulepos = 0;
        pack.packetno = 0;

 int numFrames = 0;

        outHeader->nFlags = 0;
 int err = vorbis_dsp_synthesis(mState, &pack, 1);
 if (err != 0) {
#if !defined(__arm__) && !defined(__aarch64__)
            ALOGV("vorbis_dsp_synthesis returned %d", err);
#else

             ALOGW("vorbis_dsp_synthesis returned %d", err);
 #endif
         } else {
            size_t numSamplesPerBuffer = kMaxNumSamplesPerBuffer;
            if (numSamplesPerBuffer > outHeader->nAllocLen / sizeof(int16_t)) {
                numSamplesPerBuffer = outHeader->nAllocLen / sizeof(int16_t);
                android_errorWriteLog(0x534e4554, "27833616");
            }
             numFrames = vorbis_dsp_pcmout(
                     mState, (int16_t *)outHeader->pBuffer,
                    (numSamplesPerBuffer / mVi->channels));
 
             if (numFrames < 0) {
                 ALOGE("vorbis_dsp_pcmout returned %d", numFrames);
                numFrames = 0;
 }
 }

 if (mNumFramesLeftOnPage >= 0) {
 if (numFrames > mNumFramesLeftOnPage) {
                ALOGV("discarding %d frames at end of page",
                     numFrames - mNumFramesLeftOnPage);
                numFrames = mNumFramesLeftOnPage;
 if (mSawInputEos) {
                    outHeader->nFlags = OMX_BUFFERFLAG_EOS;
                    mSignalledOutputEos = true;
 }
 }
            mNumFramesLeftOnPage -= numFrames;
 }

        outHeader->nFilledLen = numFrames * sizeof(int16_t) * mVi->channels;
        outHeader->nOffset = 0;

        outHeader->nTimeStamp =
            mAnchorTimeUs
 + (mNumFramesOutput * 1000000ll) / mVi->rate;

        mNumFramesOutput += numFrames;

 if (inHeader) {
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
