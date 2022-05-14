bool SoftVPX::outputBuffers(bool flushDecoder, bool display, bool eos, bool *portWillReset) {
 List<BufferInfo *> &outQueue = getPortQueue(1);
 BufferInfo *outInfo = NULL;
    OMX_BUFFERHEADERTYPE *outHeader = NULL;
 vpx_codec_iter_t iter = NULL;

 if (flushDecoder && mFrameParallelMode) {
 if (vpx_codec_decode((vpx_codec_ctx_t *)mCtx, NULL, 0, NULL, 0)) {
            ALOGE("Failed to flush on2 decoder.");
 return false;
 }
 }

 if (!display) {
 if (!flushDecoder) {
            ALOGE("Invalid operation.");
 return false;
 }
 while ((mImg = vpx_codec_get_frame((vpx_codec_ctx_t *)mCtx, &iter))) {
 }
 return true;
 }

 while (!outQueue.empty()) {
 if (mImg == NULL) {
            mImg = vpx_codec_get_frame((vpx_codec_ctx_t *)mCtx, &iter);
 if (mImg == NULL) {
 break;
 }
 }
 uint32_t width = mImg->d_w;
 uint32_t height = mImg->d_h;
        outInfo = *outQueue.begin();
        outHeader = outInfo->mHeader;
        CHECK_EQ(mImg->fmt, VPX_IMG_FMT_I420);
        handlePortSettingsChange(portWillReset, width, height);
 if (*portWillReset) {
 return true;
 }

        outHeader->nOffset = 0;

         outHeader->nFlags = 0;
         outHeader->nFilledLen = (outputBufferWidth() * outputBufferHeight() * 3) / 2;
         outHeader->nTimeStamp = *(OMX_TICKS *)mImg->user_priv;
        if (outputBufferSafe(outHeader)) {
             uint8_t *dst = outHeader->pBuffer;
             const uint8_t *srcY = (const uint8_t *)mImg->planes[VPX_PLANE_Y];
             const uint8_t *srcU = (const uint8_t *)mImg->planes[VPX_PLANE_U];
 const uint8_t *srcV = (const uint8_t *)mImg->planes[VPX_PLANE_V];
 size_t srcYStride = mImg->stride[VPX_PLANE_Y];
 size_t srcUStride = mImg->stride[VPX_PLANE_U];

             size_t srcVStride = mImg->stride[VPX_PLANE_V];
             copyYV12FrameToOutputBuffer(dst, srcY, srcU, srcV, srcYStride, srcUStride, srcVStride);
         } else {
             outHeader->nFilledLen = 0;
         }
 
        mImg = NULL;
        outInfo->mOwnedByUs = false;
        outQueue.erase(outQueue.begin());
        outInfo = NULL;
        notifyFillBufferDone(outHeader);
        outHeader = NULL;
 }

 if (!eos) {
 return true;
 }

 if (!outQueue.empty()) {
        outInfo = *outQueue.begin();
        outQueue.erase(outQueue.begin());
        outHeader = outInfo->mHeader;
        outHeader->nTimeStamp = 0;
        outHeader->nFilledLen = 0;
        outHeader->nFlags = OMX_BUFFERFLAG_EOS;
        outInfo->mOwnedByUs = false;
        notifyFillBufferDone(outHeader);
        mEOSStatus = OUTPUT_FRAMES_FLUSHED;
 }

     return true;
 }
