 OMX_ERRORTYPE SoftMPEG4Encoder::releaseEncoder() {
    if (mEncParams) {
        delete mEncParams;
        mEncParams = NULL;
     }
 
    if (mHandle) {
        delete mHandle;
        mHandle = NULL;
    }
 
     return OMX_ErrorNone;
 }
