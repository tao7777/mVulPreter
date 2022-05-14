 status_t OMXNodeInstance::sendCommand(
         OMX_COMMANDTYPE cmd, OMX_S32 param) {
     if (cmd == OMX_CommandStateSet) {
        // There are no configurations past first StateSet command.
         mSailed = true;
     }
     const sp<GraphicBufferSource> bufferSource(getGraphicBufferSource());
 if (bufferSource != NULL && cmd == OMX_CommandStateSet) {
 if (param == OMX_StateIdle) {
            bufferSource->omxIdle();
 } else if (param == OMX_StateLoaded) {
            bufferSource->omxLoaded();
            setGraphicBufferSource(NULL);
 }

 }

 Mutex::Autolock autoLock(mLock);

 {
 Mutex::Autolock _l(mDebugLock);
        bumpDebugLevel_l(2 /* numInputBuffers */, 2 /* numOutputBuffers */);
 }

 const char *paramString =
        cmd == OMX_CommandStateSet ? asString((OMX_STATETYPE)param) : portString(param);
    CLOG_STATE(sendCommand, "%s(%d), %s(%d)", asString(cmd), cmd, paramString, param);
    OMX_ERRORTYPE err = OMX_SendCommand(mHandle, cmd, param, NULL);
    CLOG_IF_ERROR(sendCommand, err, "%s(%d), %s(%d)", asString(cmd), cmd, paramString, param);
 return StatusFromOMXError(err);
}
