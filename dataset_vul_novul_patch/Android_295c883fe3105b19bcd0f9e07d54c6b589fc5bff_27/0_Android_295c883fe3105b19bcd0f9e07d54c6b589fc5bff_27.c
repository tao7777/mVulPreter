OMX_ERRORTYPE SoftVPXEncoder::internalGetParameter(OMX_INDEXTYPE index,
                                                   OMX_PTR param) {
 const int32_t indexFull = index;

 switch (indexFull) {
 case OMX_IndexParamVideoBitrate: {

             OMX_VIDEO_PARAM_BITRATETYPE *bitrate =
                 (OMX_VIDEO_PARAM_BITRATETYPE *)param;
 
            if (!isValidOMXParam(bitrate)) {
                return OMX_ErrorBadParameter;
            }
 
            if (bitrate->nPortIndex != kOutputPortIndex) {
                return OMX_ErrorUnsupportedIndex;
            }
 
            bitrate->nTargetBitrate = mBitrate;

            if (mBitrateControlMode == VPX_VBR) {
                bitrate->eControlRate = OMX_Video_ControlRateVariable;
            } else if (mBitrateControlMode == VPX_CBR) {
                bitrate->eControlRate = OMX_Video_ControlRateConstant;
            } else {
                return OMX_ErrorUnsupportedSetting;
            }
            return OMX_ErrorNone;
         }
 
 case OMX_IndexParamVideoVp8: {

             OMX_VIDEO_PARAM_VP8TYPE *vp8Params =
                 (OMX_VIDEO_PARAM_VP8TYPE *)param;
 
            if (!isValidOMXParam(vp8Params)) {
                return OMX_ErrorBadParameter;
            }
 
            if (vp8Params->nPortIndex != kOutputPortIndex) {
                return OMX_ErrorUnsupportedIndex;
            }

            vp8Params->eProfile = OMX_VIDEO_VP8ProfileMain;
            vp8Params->eLevel = mLevel;
            vp8Params->nDCTPartitions = mDCTPartitions;
            vp8Params->bErrorResilientMode = mErrorResilience;
            return OMX_ErrorNone;
         }
 
         case OMX_IndexParamVideoAndroidVp8Encoder: {
             OMX_VIDEO_PARAM_ANDROID_VP8ENCODERTYPE *vp8AndroidParams =
                 (OMX_VIDEO_PARAM_ANDROID_VP8ENCODERTYPE *)param;
 
            if (!isValidOMXParam(vp8AndroidParams)) {
                return OMX_ErrorBadParameter;
            }
 
            if (vp8AndroidParams->nPortIndex != kOutputPortIndex) {
                return OMX_ErrorUnsupportedIndex;
            }

            vp8AndroidParams->nKeyFrameInterval = mKeyFrameInterval;
            vp8AndroidParams->eTemporalPattern = mTemporalPatternType;
            vp8AndroidParams->nTemporalLayerCount = mTemporalLayers;
            vp8AndroidParams->nMinQuantizer = mMinQuantizer;
            vp8AndroidParams->nMaxQuantizer = mMaxQuantizer;
            memcpy(vp8AndroidParams->nTemporalLayerBitrateRatio,
                   mTemporalLayerBitrateRatio, sizeof(mTemporalLayerBitrateRatio));
            return OMX_ErrorNone;
         }
 
         default:
 return SoftVideoEncoderOMXComponent::internalGetParameter(index, param);
 }
}
