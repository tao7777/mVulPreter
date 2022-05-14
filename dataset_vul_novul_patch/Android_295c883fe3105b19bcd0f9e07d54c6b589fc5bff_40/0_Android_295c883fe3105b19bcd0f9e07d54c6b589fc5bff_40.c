OMX_ERRORTYPE SoftVideoDecoderOMXComponent::internalSetParameter(
        OMX_INDEXTYPE index, const OMX_PTR params) {
 const int32_t indexFull = index;

 switch (indexFull) {
 case OMX_IndexParamStandardComponentRole:
 {

             const OMX_PARAM_COMPONENTROLETYPE *roleParams =
                 (const OMX_PARAM_COMPONENTROLETYPE *)params;
 
            if (!isValidOMXParam(roleParams)) {
                return OMX_ErrorBadParameter;
            }

             if (strncmp((const char *)roleParams->cRole,
                         mComponentRole,
                         OMX_MAX_STRINGNAME_SIZE - 1)) {
 return OMX_ErrorUndefined;
 }

 return OMX_ErrorNone;
 }

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
 if (formatParams->eCompressionFormat != mCodingType
 || formatParams->eColorFormat != OMX_COLOR_FormatUnused) {
 return OMX_ErrorUnsupportedSetting;
 }
 } else {
 if (formatParams->eCompressionFormat != OMX_VIDEO_CodingUnused
 || formatParams->eColorFormat != OMX_COLOR_FormatYUV420Planar) {
 return OMX_ErrorUnsupportedSetting;
 }
 }

 return OMX_ErrorNone;
 }

 case kPrepareForAdaptivePlaybackIndex:

         {
             const PrepareForAdaptivePlaybackParams* adaptivePlaybackParams =
                     (const PrepareForAdaptivePlaybackParams *)params;

            if (!isValidOMXParam(adaptivePlaybackParams)) {
                return OMX_ErrorBadParameter;
            }

             mIsAdaptive = adaptivePlaybackParams->bEnable;
             if (mIsAdaptive) {
                 mAdaptiveMaxWidth = adaptivePlaybackParams->nMaxFrameWidth;
                mAdaptiveMaxHeight = adaptivePlaybackParams->nMaxFrameHeight;
                mWidth = mAdaptiveMaxWidth;
                mHeight = mAdaptiveMaxHeight;
 } else {
                mAdaptiveMaxWidth = 0;
                mAdaptiveMaxHeight = 0;
 }
            updatePortDefinitions(true /* updateCrop */, true /* updateInputSize */);
 return OMX_ErrorNone;
 }

 case OMX_IndexParamPortDefinition:

         {
             OMX_PARAM_PORTDEFINITIONTYPE *newParams =
                 (OMX_PARAM_PORTDEFINITIONTYPE *)params;

            if (!isValidOMXParam(newParams)) {
                return OMX_ErrorBadParameter;
            }

             OMX_VIDEO_PORTDEFINITIONTYPE *video_def = &newParams->format.video;
             OMX_PARAM_PORTDEFINITIONTYPE *def = &editPortInfo(newParams->nPortIndex)->mDef;
 
 uint32_t oldWidth = def->format.video.nFrameWidth;
 uint32_t oldHeight = def->format.video.nFrameHeight;
 uint32_t newWidth = video_def->nFrameWidth;
 uint32_t newHeight = video_def->nFrameHeight;
 if (newWidth != oldWidth || newHeight != oldHeight) {
 bool outputPort = (newParams->nPortIndex == kOutputPortIndex);
 if (outputPort) {
                    mWidth = newWidth;
                    mHeight = newHeight;

                    updatePortDefinitions(true /* updateCrop */, true /* updateInputSize */);
                    newParams->nBufferSize = def->nBufferSize;
 } else {
                    def->format.video.nFrameWidth = newWidth;
                    def->format.video.nFrameHeight = newHeight;
 }
 }
 return SimpleSoftOMXComponent::internalSetParameter(index, params);
 }

 default:
 return SimpleSoftOMXComponent::internalSetParameter(index, params);
 }
}
