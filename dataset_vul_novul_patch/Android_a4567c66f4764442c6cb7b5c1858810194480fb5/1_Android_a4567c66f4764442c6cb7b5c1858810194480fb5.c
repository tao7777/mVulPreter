void SoftHEVC::onQueueFilled(OMX_U32 portIndex) {
    UNUSED(portIndex);

 if (mSignalledError) {
 return;
 }
 if (mOutputPortSettingsChange != NONE) {
 return;
 }

 
     if (NULL == mCodecCtx) {
         if (OK != initDecoder()) {
             return;
         }
     }
 if (outputBufferWidth() != mStride) {
 /* Set the run-time (dynamic) parameters */
        mStride = outputBufferWidth();
        setParams(mStride);
 }

 List<BufferInfo *> &inQueue = getPortQueue(kInputPortIndex);
 List<BufferInfo *> &outQueue = getPortQueue(kOutputPortIndex);

 /* If input EOS is seen and decoder is not in flush mode,
     * set the decoder in flush mode.
     * There can be a case where EOS is sent along with last picture data
     * In that case, only after decoding that input data, decoder has to be
     * put in flush. This case is handled here  */

 if (mReceivedEOS && !mIsInFlush) {
        setFlushMode();
 }

 while (!outQueue.empty()) {
 BufferInfo *inInfo;
        OMX_BUFFERHEADERTYPE *inHeader;

 BufferInfo *outInfo;
        OMX_BUFFERHEADERTYPE *outHeader;
 size_t timeStampIx;

        inInfo = NULL;
        inHeader = NULL;

 if (!mIsInFlush) {
 if (!inQueue.empty()) {
                inInfo = *inQueue.begin();
                inHeader = inInfo->mHeader;
 } else {
 break;
 }
 }

        outInfo = *outQueue.begin();
        outHeader = outInfo->mHeader;
        outHeader->nFlags = 0;
        outHeader->nTimeStamp = 0;
        outHeader->nOffset = 0;

 if (inHeader != NULL && (inHeader->nFlags & OMX_BUFFERFLAG_EOS)) {
            mReceivedEOS = true;
 if (inHeader->nFilledLen == 0) {
                inQueue.erase(inQueue.begin());
                inInfo->mOwnedByUs = false;
                notifyEmptyBufferDone(inHeader);
                inHeader = NULL;
                setFlushMode();
 }
 }

 /* Get a free slot in timestamp array to hold input timestamp */
 {
 size_t i;
            timeStampIx = 0;
 for (i = 0; i < MAX_TIME_STAMPS; i++) {
 if (!mTimeStampsValid[i]) {
                    timeStampIx = i;
 break;
 }
 }
 if (inHeader != NULL) {
                mTimeStampsValid[timeStampIx] = true;
                mTimeStamps[timeStampIx] = inHeader->nTimeStamp;
 }
 }

 {
 ivd_video_decode_ip_t s_dec_ip;
 ivd_video_decode_op_t s_dec_op;
            WORD32 timeDelay, timeTaken;
 size_t sizeY, sizeUV;

 if (!setDecodeArgs(&s_dec_ip, &s_dec_op, inHeader, outHeader, timeStampIx)) {
                ALOGE("Decoder arg setup failed");
                notify(OMX_EventError, OMX_ErrorUndefined, 0, NULL);
                mSignalledError = true;
 return;
 }

            GETTIME(&mTimeStart, NULL);
 /* Compute time elapsed between end of previous decode()
             * to start of current decode() */
            TIME_DIFF(mTimeEnd, mTimeStart, timeDelay);


             IV_API_CALL_STATUS_T status;
             status = ivdec_api_function(mCodecCtx, (void *)&s_dec_ip, (void *)&s_dec_op);
 
             bool resChanged = (IVD_RES_CHANGED == (s_dec_op.u4_error_code & 0xFF));
 
             GETTIME(&mTimeEnd, NULL);
 /* Compute time taken for decode() */
            TIME_DIFF(mTimeStart, mTimeEnd, timeTaken);

            ALOGV("timeTaken=%6d delay=%6d numBytes=%6d", timeTaken, timeDelay,
                   s_dec_op.u4_num_bytes_consumed);
 if (s_dec_op.u4_frame_decoded_flag && !mFlushNeeded) {
                mFlushNeeded = true;
 }

 if ((inHeader != NULL) && (1 != s_dec_op.u4_frame_decoded_flag)) {
 /* If the input did not contain picture data, then ignore
                 * the associated timestamp */
                mTimeStampsValid[timeStampIx] = false;
 }

 if (mChangingResolution && !s_dec_op.u4_output_present) {
                mChangingResolution = false;
                resetDecoder();
                resetPlugin();
 continue;
 }

 if (resChanged) {
                mChangingResolution = true;
 if (mFlushNeeded) {
                    setFlushMode();
 }
 continue;
 }

 if ((0 < s_dec_op.u4_pic_wd) && (0 < s_dec_op.u4_pic_ht)) {
 uint32_t width = s_dec_op.u4_pic_wd;
 uint32_t height = s_dec_op.u4_pic_ht;
 bool portWillReset = false;
                handlePortSettingsChange(&portWillReset, width, height);

 if (portWillReset) {
                    resetDecoder();
 return;
 }
 }

 if (s_dec_op.u4_output_present) {
                outHeader->nFilledLen = (outputBufferWidth() * outputBufferHeight() * 3) / 2;

                outHeader->nTimeStamp = mTimeStamps[s_dec_op.u4_ts];
                mTimeStampsValid[s_dec_op.u4_ts] = false;

                outInfo->mOwnedByUs = false;
                outQueue.erase(outQueue.begin());
                outInfo = NULL;
                notifyFillBufferDone(outHeader);
                outHeader = NULL;
 } else {
 /* If in flush mode and no output is returned by the codec,
                 * then come out of flush mode */
                mIsInFlush = false;

 /* If EOS was recieved on input port and there is no output
                 * from the codec, then signal EOS on output port */
 if (mReceivedEOS) {
                    outHeader->nFilledLen = 0;
                    outHeader->nFlags |= OMX_BUFFERFLAG_EOS;

                    outInfo->mOwnedByUs = false;
                    outQueue.erase(outQueue.begin());
                    outInfo = NULL;
                    notifyFillBufferDone(outHeader);
                    outHeader = NULL;
                    resetPlugin();
 }
 }
 }

 if (inHeader != NULL) {
            inInfo->mOwnedByUs = false;
            inQueue.erase(inQueue.begin());
            inInfo = NULL;
            notifyEmptyBufferDone(inHeader);
            inHeader = NULL;
 }
 }
}
