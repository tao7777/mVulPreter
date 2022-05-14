 OMX_ERRORTYPE SoftMPEG4Encoder::releaseEncoder() {
    if (!mStarted) {
        return OMX_ErrorNone;
     }
 
    PVCleanUpVideoEncoder(mHandle);
    free(mInputFrameData);
    mInputFrameData = NULL;
    delete mEncParams;
    mEncParams = NULL;
    delete mHandle;
    mHandle = NULL;
    mStarted = false;
 
     return OMX_ErrorNone;
 }
