void SoftOpus::onQueueFilled(OMX_U32 portIndex) {
 List<BufferInfo *> &inQueue = getPortQueue(0);
 List<BufferInfo *> &outQueue = getPortQueue(1);

 if (mOutputPortSettingsChange != NONE) {
 return;
 }

 if (portIndex == 0 && mInputBufferCount < 3) {
 BufferInfo *info = *inQueue.begin();
        OMX_BUFFERHEADERTYPE *header = info->mHeader;

 const uint8_t *data = header->pBuffer + header->nOffset;
 size_t size = header->nFilledLen;

 if (mInputBufferCount == 0) {
            CHECK(mHeader == NULL);
            mHeader = new OpusHeader();
            memset(mHeader, 0, sizeof(*mHeader));
 if (!ParseOpusHeader(data, size, mHeader)) {
                ALOGV("Parsing Opus Header failed.");
                notify(OMX_EventError, OMX_ErrorUndefined, 0, NULL);
 return;
 }

 uint8_t channel_mapping[kMaxChannels] = {0};
 if (mHeader->channels <= kMaxChannelsWithDefaultLayout) {
                memcpy(&channel_mapping,
                       kDefaultOpusChannelLayout,
                       kMaxChannelsWithDefaultLayout);
 } else {
                memcpy(&channel_mapping,
                       mHeader->stream_map,
                       mHeader->channels);
 }

 int status = OPUS_INVALID_STATE;
            mDecoder = opus_multistream_decoder_create(kRate,
                                                       mHeader->channels,
                                                       mHeader->num_streams,
                                                       mHeader->num_coupled,
                                                       channel_mapping,
 &status);
 if (!mDecoder || status != OPUS_OK) {
                ALOGV("opus_multistream_decoder_create failed status=%s",
                      opus_strerror(status));
                notify(OMX_EventError, OMX_ErrorUndefined, 0, NULL);
 return;
 }
            status =
                opus_multistream_decoder_ctl(mDecoder,
                                             OPUS_SET_GAIN(mHeader->gain_db));
 if (status != OPUS_OK) {
                ALOGV("Failed to set OPUS header gain; status=%s",
                      opus_strerror(status));
                notify(OMX_EventError, OMX_ErrorUndefined, 0, NULL);
 return;
 }
 } else if (mInputBufferCount == 1) {
            mCodecDelay = ns_to_samples(
 *(reinterpret_cast<int64_t*>(header->pBuffer +
                                                           header->nOffset)),
                              kRate);
            mSamplesToDiscard = mCodecDelay;
 } else {
            mSeekPreRoll = ns_to_samples(
 *(reinterpret_cast<int64_t*>(header->pBuffer +
                                                            header->nOffset)),
                               kRate);
            notify(OMX_EventPortSettingsChanged, 1, 0, NULL);
            mOutputPortSettingsChange = AWAITING_DISABLED;
 }

        inQueue.erase(inQueue.begin());
        info->mOwnedByUs = false;
        notifyEmptyBufferDone(header);
 ++mInputBufferCount;
 return;
 }

 while (!inQueue.empty() && !outQueue.empty()) {
 BufferInfo *inInfo = *inQueue.begin();
        OMX_BUFFERHEADERTYPE *inHeader = inInfo->mHeader;

 if (inHeader->nFlags & OMX_BUFFERFLAG_CODECCONFIG) {
            inQueue.erase(inQueue.begin());
            inInfo->mOwnedByUs = false;
            notifyEmptyBufferDone(inHeader);
 return;
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
            mNumFramesOutput = 0;
 }

 if (inHeader->nTimeStamp == 0) {
            mSamplesToDiscard = mCodecDelay;
 }

 
         const uint8_t *data = inHeader->pBuffer + inHeader->nOffset;
         const uint32_t size = inHeader->nFilledLen;
 
         int numFrames = opus_multistream_decode(mDecoder,
                                                 data,
                                                 size,
                                                 (int16_t *)outHeader->pBuffer,
                                                kMaxOpusOutputPacketSizeSamples,
                                                 0);
         if (numFrames < 0) {
             ALOGE("opus_multistream_decode returned %d", numFrames);
            notify(OMX_EventError, OMX_ErrorUndefined, 0, NULL);
 return;
 }

        outHeader->nOffset = 0;
 if (mSamplesToDiscard > 0) {
 if (mSamplesToDiscard > numFrames) {
                mSamplesToDiscard -= numFrames;
                numFrames = 0;
 } else {
                numFrames -= mSamplesToDiscard;
                outHeader->nOffset = mSamplesToDiscard * sizeof(int16_t) *
                                     mHeader->channels;
                mSamplesToDiscard = 0;
 }
 }

        outHeader->nFilledLen = numFrames * sizeof(int16_t) * mHeader->channels;
        outHeader->nFlags = 0;

        outHeader->nTimeStamp = mAnchorTimeUs +
 (mNumFramesOutput * 1000000ll) /
                                kRate;

        mNumFramesOutput += numFrames;

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

 ++mInputBufferCount;
 }
}
