OMX_ERRORTYPE SoftVideoDecoderOMXComponent::internalGetParameter(
        OMX_INDEXTYPE index, OMX_PTR params) {
 switch (index) {
 case OMX_IndexParamVideoPortFormat:
 {

             OMX_VIDEO_PARAM_PORTFORMATTYPE *formatParams =
                 (OMX_VIDEO_PARAM_PORTFORMATTYPE *)params;
 
            if (!isValidOMXParam(formatParams)) {
                return OMX_ErrorBadParameter;
            }

             if (formatParams->nPortIndex > kMaxPortIndex) {
                 return OMX_ErrorBadPortIndex;
             }

 if (formatParams->nIndex != 0) {
 return OMX_ErrorNoMore;
 }

 if (formatParams->nPortIndex == kInputPortIndex) {
                formatParams->eCompressionFormat = mCodingType;
                formatParams->eColorFormat = OMX_COLOR_FormatUnused;
                formatParams->xFramerate = 0;
 } else {
                CHECK_EQ(formatParams->nPortIndex, 1u);

                formatParams->eCompressionFormat = OMX_VIDEO_CodingUnused;
                formatParams->eColorFormat = OMX_COLOR_FormatYUV420Planar;
                formatParams->xFramerate = 0;
 }

 return OMX_ErrorNone;
 }

 case OMX_IndexParamVideoProfileLevelQuerySupported:
 {

             OMX_VIDEO_PARAM_PROFILELEVELTYPE *profileLevel =
                   (OMX_VIDEO_PARAM_PROFILELEVELTYPE *) params;
 
            if (!isValidOMXParam(profileLevel)) {
                return OMX_ErrorBadParameter;
            }

             if (profileLevel->nPortIndex != kInputPortIndex) {
                 ALOGE("Invalid port index: %" PRIu32, profileLevel->nPortIndex);
                 return OMX_ErrorUnsupportedIndex;
 }

 if (profileLevel->nProfileIndex >= mNumProfileLevels) {
 return OMX_ErrorNoMore;
 }

            profileLevel->eProfile = mProfileLevels[profileLevel->nProfileIndex].mProfile;
            profileLevel->eLevel   = mProfileLevels[profileLevel->nProfileIndex].mLevel;
 return OMX_ErrorNone;
 }

 default:
 return SimpleSoftOMXComponent::internalGetParameter(index, params);
 }
}
