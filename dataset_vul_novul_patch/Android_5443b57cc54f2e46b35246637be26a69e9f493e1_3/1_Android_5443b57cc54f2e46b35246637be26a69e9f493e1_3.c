void SoftVPXEncoder::onQueueFilled(OMX_U32 portIndex) {
 if (mCodecContext == NULL) {
 if (OK != initEncoder()) {
            ALOGE("Failed to initialize encoder");
            notify(OMX_EventError,
                   OMX_ErrorUndefined,
 0, // Extra notification data
                   NULL); // Notification data pointer
 return;
 }
 }

 vpx_codec_err_t codec_return;
 List<BufferInfo *> &inputBufferInfoQueue = getPortQueue(kInputPortIndex);
 List<BufferInfo *> &outputBufferInfoQueue = getPortQueue(kOutputPortIndex);

 while (!inputBufferInfoQueue.empty() && !outputBufferInfoQueue.empty()) {
 BufferInfo *inputBufferInfo = *inputBufferInfoQueue.begin();
        OMX_BUFFERHEADERTYPE *inputBufferHeader = inputBufferInfo->mHeader;

 BufferInfo *outputBufferInfo = *outputBufferInfoQueue.begin();
        OMX_BUFFERHEADERTYPE *outputBufferHeader = outputBufferInfo->mHeader;

 if (inputBufferHeader->nFlags & OMX_BUFFERFLAG_EOS) {
            inputBufferInfoQueue.erase(inputBufferInfoQueue.begin());
            inputBufferInfo->mOwnedByUs = false;
            notifyEmptyBufferDone(inputBufferHeader);

            outputBufferHeader->nFilledLen = 0;
            outputBufferHeader->nFlags = OMX_BUFFERFLAG_EOS;

            outputBufferInfoQueue.erase(outputBufferInfoQueue.begin());
            outputBufferInfo->mOwnedByUs = false;
            notifyFillBufferDone(outputBufferHeader);
 return;
 }

 const uint8_t *source =
            inputBufferHeader->pBuffer + inputBufferHeader->nOffset;

 size_t frameSize = mWidth * mHeight * 3 / 2;
 if (mInputDataIsMeta) {
            source = extractGraphicBuffer(
                    mConversionBuffer, frameSize,
                    source, inputBufferHeader->nFilledLen,
                    mWidth, mHeight);
 if (source == NULL) {
                ALOGE("Unable to extract gralloc buffer in metadata mode");
                notify(OMX_EventError, OMX_ErrorUndefined, 0, 0);
 return;
 }
 } else {
 if (inputBufferHeader->nFilledLen < frameSize) {
                android_errorWriteLog(0x534e4554, "27569635");
                notify(OMX_EventError, OMX_ErrorUndefined, 0, 0);
 return;
 } else if (inputBufferHeader->nFilledLen > frameSize) {
                ALOGW("Input buffer contains too many pixels");
 }

 if (mColorFormat == OMX_COLOR_FormatYUV420SemiPlanar) {
 ConvertYUV420SemiPlanarToYUV420Planar(
                        source, mConversionBuffer, mWidth, mHeight);

                source = mConversionBuffer;
 }
 }
 vpx_image_t raw_frame;
        vpx_img_wrap(&raw_frame, VPX_IMG_FMT_I420, mWidth, mHeight,
                     kInputBufferAlignment, (uint8_t *)source);

 vpx_enc_frame_flags_t flags = 0;
 if (mTemporalPatternLength > 0) {
            flags = getEncodeFlags();
 }
 if (mKeyFrameRequested) {
            flags |= VPX_EFLAG_FORCE_KF;
            mKeyFrameRequested = false;
 }

 if (mBitrateUpdated) {
            mCodecConfiguration->rc_target_bitrate = mBitrate/1000;
 vpx_codec_err_t res = vpx_codec_enc_config_set(mCodecContext,
                                                           mCodecConfiguration);
 if (res != VPX_CODEC_OK) {
                ALOGE("vp8 encoder failed to update bitrate: %s",
                      vpx_codec_err_to_string(res));
                notify(OMX_EventError,
                       OMX_ErrorUndefined,
 0, // Extra notification data
                       NULL); // Notification data pointer
 }
            mBitrateUpdated = false;
 }

 uint32_t frameDuration;

         if (inputBufferHeader->nTimeStamp > mLastTimestamp) {
             frameDuration = (uint32_t)(inputBufferHeader->nTimeStamp - mLastTimestamp);
         } else {
            frameDuration = (uint32_t)(((uint64_t)1000000 << 16) / mFramerate);
         }
         mLastTimestamp = inputBufferHeader->nTimeStamp;
         codec_return = vpx_codec_encode(
                mCodecContext,
 &raw_frame,
                inputBufferHeader->nTimeStamp, // in timebase units
                frameDuration, // frame duration in timebase units
                flags, // frame flags
                VPX_DL_REALTIME); // encoding deadline
 if (codec_return != VPX_CODEC_OK) {
            ALOGE("vpx encoder failed to encode frame");
            notify(OMX_EventError,
                   OMX_ErrorUndefined,
 0, // Extra notification data
                   NULL); // Notification data pointer
 return;
 }

 vpx_codec_iter_t encoded_packet_iterator = NULL;
 const vpx_codec_cx_pkt_t* encoded_packet;

 while ((encoded_packet = vpx_codec_get_cx_data(
                        mCodecContext, &encoded_packet_iterator))) {
 if (encoded_packet->kind == VPX_CODEC_CX_FRAME_PKT) {
                outputBufferHeader->nTimeStamp = encoded_packet->data.frame.pts;
                outputBufferHeader->nFlags = 0;
 if (encoded_packet->data.frame.flags & VPX_FRAME_IS_KEY)
                    outputBufferHeader->nFlags |= OMX_BUFFERFLAG_SYNCFRAME;
                outputBufferHeader->nOffset = 0;
                outputBufferHeader->nFilledLen = encoded_packet->data.frame.sz;
 if (outputBufferHeader->nFilledLen > outputBufferHeader->nAllocLen) {
                    android_errorWriteLog(0x534e4554, "27569635");
                    notify(OMX_EventError, OMX_ErrorUndefined, 0, 0);
 return;
 }
                memcpy(outputBufferHeader->pBuffer,
                       encoded_packet->data.frame.buf,
                       encoded_packet->data.frame.sz);
                outputBufferInfo->mOwnedByUs = false;
                outputBufferInfoQueue.erase(outputBufferInfoQueue.begin());
                notifyFillBufferDone(outputBufferHeader);
 }
 }

        inputBufferInfo->mOwnedByUs = false;
        inputBufferInfoQueue.erase(inputBufferInfoQueue.begin());
        notifyEmptyBufferDone(inputBufferHeader);

     }
 }
