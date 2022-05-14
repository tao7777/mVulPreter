 void SoftAVC::drainAllOutputBuffers(bool eos) {
 List<BufferInfo *> &outQueue = getPortQueue(kOutputPortIndex);
    H264SwDecPicture decodedPicture;

 if (mHeadersDecoded) {
 while (!outQueue.empty()
 && H264SWDEC_PIC_RDY == H264SwDecNextPicture(

                     mHandle, &decodedPicture, eos /* flush */)) {
             int32_t picId = decodedPicture.picId;
             uint8_t *data = (uint8_t *) decodedPicture.pOutputPicture;
            if (!drainOneOutputBuffer(picId, data)) {
                ALOGE("Drain failed");
                notify(OMX_EventError, OMX_ErrorUndefined, 0, NULL);
                mSignalledError = true;
                return;
            }
         }
     }
 
 if (!eos) {
 return;
 }

 while (!outQueue.empty()) {
 BufferInfo *outInfo = *outQueue.begin();
        outQueue.erase(outQueue.begin());
        OMX_BUFFERHEADERTYPE *outHeader = outInfo->mHeader;

        outHeader->nTimeStamp = 0;
        outHeader->nFilledLen = 0;
        outHeader->nFlags = OMX_BUFFERFLAG_EOS;

        outInfo->mOwnedByUs = false;
        notifyFillBufferDone(outHeader);

        mEOSStatus = OUTPUT_FRAMES_FLUSHED;
 }
}
