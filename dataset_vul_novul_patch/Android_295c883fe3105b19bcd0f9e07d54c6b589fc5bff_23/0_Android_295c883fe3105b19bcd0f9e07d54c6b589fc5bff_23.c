OMX_ERRORTYPE SoftMPEG4Encoder::internalGetParameter(
        OMX_INDEXTYPE index, OMX_PTR params) {
 switch (index) {
 case OMX_IndexParamVideoBitrate:
 {

             OMX_VIDEO_PARAM_BITRATETYPE *bitRate =
                 (OMX_VIDEO_PARAM_BITRATETYPE *) params;
 
            if (!isValidOMXParam(bitRate)) {
                return OMX_ErrorBadParameter;
            }

             if (bitRate->nPortIndex != 1) {
                 return OMX_ErrorUndefined;
             }

            bitRate->eControlRate = OMX_Video_ControlRateVariable;
            bitRate->nTargetBitrate = mBitrate;
 return OMX_ErrorNone;
 }

 case OMX_IndexParamVideoH263:
 {

             OMX_VIDEO_PARAM_H263TYPE *h263type =
                 (OMX_VIDEO_PARAM_H263TYPE *)params;
 
            if (!isValidOMXParam(h263type)) {
                return OMX_ErrorBadParameter;
            }

             if (h263type->nPortIndex != 1) {
                 return OMX_ErrorUndefined;
             }

            h263type->nAllowedPictureTypes =
 (OMX_VIDEO_PictureTypeI | OMX_VIDEO_PictureTypeP);
            h263type->eProfile = OMX_VIDEO_H263ProfileBaseline;
            h263type->eLevel = OMX_VIDEO_H263Level45;
            h263type->bPLUSPTYPEAllowed = OMX_FALSE;
            h263type->bForceRoundingTypeToZero = OMX_FALSE;
            h263type->nPictureHeaderRepetition = 0;
            h263type->nGOBHeaderInterval = 0;

 return OMX_ErrorNone;
 }

 case OMX_IndexParamVideoMpeg4:
 {

             OMX_VIDEO_PARAM_MPEG4TYPE *mpeg4type =
                 (OMX_VIDEO_PARAM_MPEG4TYPE *)params;
 
            if (!isValidOMXParam(mpeg4type)) {
                return OMX_ErrorBadParameter;
            }

             if (mpeg4type->nPortIndex != 1) {
                 return OMX_ErrorUndefined;
             }

            mpeg4type->eProfile = OMX_VIDEO_MPEG4ProfileCore;
            mpeg4type->eLevel = OMX_VIDEO_MPEG4Level2;
            mpeg4type->nAllowedPictureTypes =
 (OMX_VIDEO_PictureTypeI | OMX_VIDEO_PictureTypeP);
            mpeg4type->nBFrames = 0;
            mpeg4type->nIDCVLCThreshold = 0;
            mpeg4type->bACPred = OMX_TRUE;
            mpeg4type->nMaxPacketSize = 256;
            mpeg4type->nTimeIncRes = 1000;
            mpeg4type->nHeaderExtension = 0;
            mpeg4type->bReversibleVLC = OMX_FALSE;

 return OMX_ErrorNone;
 }

 default:
 return SoftVideoEncoderOMXComponent::internalGetParameter(index, params);
 }
}
