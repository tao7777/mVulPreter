void SoftVPX::onQueueFilled(OMX_U32 /* portIndex */) {
 if (mOutputPortSettingsChange != NONE) {
 return;
 }

 List<BufferInfo *> &inQueue = getPortQueue(0);
 List<BufferInfo *> &outQueue = getPortQueue(1);
 bool EOSseen = false;

 while (!inQueue.empty() && !outQueue.empty()) {
 BufferInfo *inInfo = *inQueue.begin();
        OMX_BUFFERHEADERTYPE *inHeader = inInfo->mHeader;

 BufferInfo *outInfo = *outQueue.begin();
        OMX_BUFFERHEADERTYPE *outHeader = outInfo->mHeader;

 if (inHeader->nFlags & OMX_BUFFERFLAG_EOS) {
 EOSseen = true;
 if (inHeader->nFilledLen == 0) {
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
 }

 if (mImg == NULL) {
 if (vpx_codec_decode(
 (vpx_codec_ctx_t *)mCtx,
                        inHeader->pBuffer + inHeader->nOffset,
                        inHeader->nFilledLen,
                        NULL,
 0)) {
                ALOGE("on2 decoder failed to decode frame.");

                notify(OMX_EventError, OMX_ErrorUndefined, 0, NULL);
 return;
 }
 vpx_codec_iter_t iter = NULL;
            mImg = vpx_codec_get_frame((vpx_codec_ctx_t *)mCtx, &iter);

         }
 
         if (mImg != NULL) {
            CHECK_EQ(mImg->fmt, VPX_IMG_FMT_I420);
 
             uint32_t width = mImg->d_w;
             uint32_t height = mImg->d_h;
 bool portWillReset = false;
            handlePortSettingsChange(&portWillReset, width, height);
 if (portWillReset) {
 return;
 }

            outHeader->nOffset = 0;
            outHeader->nFilledLen = (outputBufferWidth() * outputBufferHeight() * 3) / 2;
            outHeader->nFlags = EOSseen ? OMX_BUFFERFLAG_EOS : 0;

             outHeader->nTimeStamp = inHeader->nTimeStamp;
 
             uint8_t *dst = outHeader->pBuffer;
            const uint8_t *srcY = (const uint8_t *)mImg->planes[VPX_PLANE_Y];
            const uint8_t *srcU = (const uint8_t *)mImg->planes[VPX_PLANE_U];
            const uint8_t *srcV = (const uint8_t *)mImg->planes[VPX_PLANE_V];
            size_t srcYStride = mImg->stride[VPX_PLANE_Y];
            size_t srcUStride = mImg->stride[VPX_PLANE_U];
            size_t srcVStride = mImg->stride[VPX_PLANE_V];
             copyYV12FrameToOutputBuffer(dst, srcY, srcU, srcV, srcYStride, srcUStride, srcVStride);
 
             mImg = NULL;
            outInfo->mOwnedByUs = false;
            outQueue.erase(outQueue.begin());
            outInfo = NULL;
            notifyFillBufferDone(outHeader);
            outHeader = NULL;
 }

        inInfo->mOwnedByUs = false;
        inQueue.erase(inQueue.begin());
        inInfo = NULL;
        notifyEmptyBufferDone(inHeader);
        inHeader = NULL;
 }
}
