void SoftAVC::onQueueFilled(OMX_U32 /* portIndex */) {
 if (mSignalledError || mOutputPortSettingsChange != NONE) {
 return;
 }

 if (mEOSStatus == OUTPUT_FRAMES_FLUSHED) {
 return;
 }

 List<BufferInfo *> &inQueue = getPortQueue(kInputPortIndex);
 List<BufferInfo *> &outQueue = getPortQueue(kOutputPortIndex);

 if (mHeadersDecoded) {

        drainAllOutputBuffers(false /* eos */);
 }

    H264SwDecRet ret = H264SWDEC_PIC_RDY;
 bool portWillReset = false;
 while ((mEOSStatus != INPUT_DATA_AVAILABLE || !inQueue.empty())
 && outQueue.size() == kNumOutputBuffers) {

 if (mEOSStatus == INPUT_EOS_SEEN) {
            drainAllOutputBuffers(true /* eos */);
 return;
 }

 BufferInfo *inInfo = *inQueue.begin();
        OMX_BUFFERHEADERTYPE *inHeader = inInfo->mHeader;
 ++mPicId;

        OMX_BUFFERHEADERTYPE *header = new OMX_BUFFERHEADERTYPE;
        memset(header, 0, sizeof(OMX_BUFFERHEADERTYPE));
        header->nTimeStamp = inHeader->nTimeStamp;
        header->nFlags = inHeader->nFlags;
 if (header->nFlags & OMX_BUFFERFLAG_EOS) {
            mEOSStatus = INPUT_EOS_SEEN;
 }
        mPicToHeaderMap.add(mPicId, header);
        inQueue.erase(inQueue.begin());

        H264SwDecInput inPicture;
        H264SwDecOutput outPicture;
        memset(&inPicture, 0, sizeof(inPicture));
        inPicture.dataLen = inHeader->nFilledLen;
        inPicture.pStream = inHeader->pBuffer + inHeader->nOffset;
        inPicture.picId = mPicId;
        inPicture.intraConcealmentMethod = 1;
        H264SwDecPicture decodedPicture;

 while (inPicture.dataLen > 0) {
            ret = H264SwDecDecode(mHandle, &inPicture, &outPicture);
 if (ret == H264SWDEC_HDRS_RDY_BUFF_NOT_EMPTY ||
                ret == H264SWDEC_PIC_RDY_BUFF_NOT_EMPTY) {
                inPicture.dataLen -= (u32)(outPicture.pStrmCurrPos - inPicture.pStream);
                inPicture.pStream = outPicture.pStrmCurrPos;
 if (ret == H264SWDEC_HDRS_RDY_BUFF_NOT_EMPTY) {
                    mHeadersDecoded = true;
                    H264SwDecInfo decoderInfo;
                    CHECK(H264SwDecGetInfo(mHandle, &decoderInfo) == H264SWDEC_OK);

 SoftVideoDecoderOMXComponent::CropSettingsMode cropSettingsMode =
                        handleCropParams(decoderInfo);
                    handlePortSettingsChange(
 &portWillReset, decoderInfo.picWidth, decoderInfo.picHeight,
                            cropSettingsMode);
 }
 } else {
 if (portWillReset) {
 if (H264SwDecNextPicture(mHandle, &decodedPicture, 0)
 == H264SWDEC_PIC_RDY) {

                        saveFirstOutputBuffer(
                            decodedPicture.picId,
 (uint8_t *)decodedPicture.pOutputPicture);
 }
 }
                inPicture.dataLen = 0;
 if (ret < 0) {
                    ALOGE("Decoder failed: %d", ret);

                    notify(OMX_EventError, OMX_ErrorUndefined,
                           ERROR_MALFORMED, NULL);

                    mSignalledError = true;
 return;
 }
 }
 }
        inInfo->mOwnedByUs = false;
        notifyEmptyBufferDone(inHeader);

 if (portWillReset) {
 return;

         }
 
         if (mFirstPicture && !outQueue.empty()) {
            drainOneOutputBuffer(mFirstPictureId, mFirstPicture);
             delete[] mFirstPicture;
             mFirstPicture = NULL;
             mFirstPictureId = -1;
 }

        drainAllOutputBuffers(false /* eos */);
 }
}
