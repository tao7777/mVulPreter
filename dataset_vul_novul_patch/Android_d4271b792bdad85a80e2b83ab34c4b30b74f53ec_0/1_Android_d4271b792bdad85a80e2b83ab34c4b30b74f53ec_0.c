OMX_ERRORTYPE SoftMPEG4Encoder::initEncParams() {
    CHECK(mHandle != NULL);
    memset(mHandle, 0, sizeof(tagvideoEncControls));

    CHECK(mEncParams != NULL);
    memset(mEncParams, 0, sizeof(tagvideoEncOptions));
 if (!PVGetDefaultEncOption(mEncParams, 0)) {

         ALOGE("Failed to get default encoding parameters");
         return OMX_ErrorUndefined;
     }
     mEncParams->encMode = mEncodeMode;
     mEncParams->encWidth[0] = mWidth;
     mEncParams->encHeight[0] = mHeight;
    mEncParams->encFrameRate[0] = mFramerate >> 16; // mFramerate is in Q16 format
    mEncParams->rcType = VBR_1;
    mEncParams->vbvDelay = 5.0f;

    mEncParams->profile_level = CORE_PROFILE_LEVEL2;
    mEncParams->packetSize = 32;
    mEncParams->rvlcEnable = PV_OFF;
    mEncParams->numLayers = 1;
    mEncParams->timeIncRes = 1000;
    mEncParams->tickPerSrc = ((int64_t)mEncParams->timeIncRes << 16) / mFramerate;

    mEncParams->bitRate[0] = mBitrate;
    mEncParams->iQuant[0] = 15;
    mEncParams->pQuant[0] = 12;
    mEncParams->quantType[0] = 0;
    mEncParams->noFrameSkipped = PV_OFF;

 if (mColorFormat != OMX_COLOR_FormatYUV420Planar || mInputDataIsMeta) {
        free(mInputFrameData);
        mInputFrameData = NULL;
 if (((uint64_t)mWidth * mHeight) > ((uint64_t)INT32_MAX / 3)) {
            ALOGE("b/25812794, Buffer size is too big.");
 return OMX_ErrorBadParameter;
 }
        mInputFrameData =
 (uint8_t *) malloc((mWidth * mHeight * 3 ) >> 1);
        CHECK(mInputFrameData != NULL);
 }

 if (mWidth % 16 != 0 || mHeight % 16 != 0) {
        ALOGE("Video frame size %dx%d must be a multiple of 16",
            mWidth, mHeight);
 return OMX_ErrorBadParameter;
 }

 if (mIDRFrameRefreshIntervalInSec < 0) {
        mEncParams->intraPeriod = -1;
 } else if (mIDRFrameRefreshIntervalInSec == 0) {
        mEncParams->intraPeriod = 1; // All I frames
 } else {
        mEncParams->intraPeriod =
 (mIDRFrameRefreshIntervalInSec * mFramerate) >> 16;
 }

    mEncParams->numIntraMB = 0;
    mEncParams->sceneDetect = PV_ON;
    mEncParams->searchRange = 16;
    mEncParams->mv8x8Enable = PV_OFF;
    mEncParams->gobHeaderInterval = 0;
    mEncParams->useACPred = PV_ON;
    mEncParams->intraDCVlcTh = 0;

 return OMX_ErrorNone;
}
