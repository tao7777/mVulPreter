status_t OMXNodeInstance::prepareForAdaptivePlayback(

         OMX_U32 portIndex, OMX_BOOL enable, OMX_U32 maxFrameWidth,
         OMX_U32 maxFrameHeight) {
     Mutex::Autolock autolock(mLock);
    if (mSailed) {
        android_errorWriteLog(0x534e4554, "29422020");
        return INVALID_OPERATION;
    }
     CLOG_CONFIG(prepareForAdaptivePlayback, "%s:%u en=%d max=%ux%u",
             portString(portIndex), portIndex, enable, maxFrameWidth, maxFrameHeight);
 
    OMX_INDEXTYPE index;
    OMX_STRING name = const_cast<OMX_STRING>(
 "OMX.google.android.index.prepareForAdaptivePlayback");

    OMX_ERRORTYPE err = OMX_GetExtensionIndex(mHandle, name, &index);
 if (err != OMX_ErrorNone) {
        CLOG_ERROR_IF(enable, getExtensionIndex, err, "%s", name);
 return StatusFromOMXError(err);
 }

 PrepareForAdaptivePlaybackParams params;
 InitOMXParams(&params);
    params.nPortIndex = portIndex;
    params.bEnable = enable;
    params.nMaxFrameWidth = maxFrameWidth;
    params.nMaxFrameHeight = maxFrameHeight;

    err = OMX_SetParameter(mHandle, index, &params);
    CLOG_IF_ERROR(setParameter, err, "%s(%#x): %s:%u en=%d max=%ux%u", name, index,
            portString(portIndex), portIndex, enable, maxFrameWidth, maxFrameHeight);
 return StatusFromOMXError(err);
}
