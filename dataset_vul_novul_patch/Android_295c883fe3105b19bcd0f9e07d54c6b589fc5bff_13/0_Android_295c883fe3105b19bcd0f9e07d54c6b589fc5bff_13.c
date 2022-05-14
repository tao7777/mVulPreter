OMX_ERRORTYPE SoftAVCEncoder::internalSetParameter(
        OMX_INDEXTYPE index, const OMX_PTR params) {
 int32_t indexFull = index;

 switch (indexFull) {
 case OMX_IndexParamVideoBitrate:
 {

             OMX_VIDEO_PARAM_BITRATETYPE *bitRate =
                 (OMX_VIDEO_PARAM_BITRATETYPE *) params;
 
            if (!isValidOMXParam(bitRate)) {
                return OMX_ErrorBadParameter;
            }

             if (bitRate->nPortIndex != 1 ||
                 bitRate->eControlRate != OMX_Video_ControlRateVariable) {
                 return OMX_ErrorUndefined;
 }

            mBitrate = bitRate->nTargetBitrate;
 return OMX_ErrorNone;
 }

 case OMX_IndexParamVideoAvc:
 {

             OMX_VIDEO_PARAM_AVCTYPE *avcType =
                 (OMX_VIDEO_PARAM_AVCTYPE *)params;
 
            if (!isValidOMXParam(avcType)) {
                return OMX_ErrorBadParameter;
            }

             if (avcType->nPortIndex != 1) {
                 return OMX_ErrorUndefined;
             }

 if (avcType->eProfile != OMX_VIDEO_AVCProfileBaseline ||
                avcType->nRefFrames != 1 ||
                avcType->nBFrames != 0 ||
                avcType->bUseHadamard != OMX_TRUE ||
 (avcType->nAllowedPictureTypes & OMX_VIDEO_PictureTypeB) != 0 ||
                avcType->nRefIdx10ActiveMinus1 != 0 ||
                avcType->nRefIdx11ActiveMinus1 != 0 ||
                avcType->bWeightedPPrediction != OMX_FALSE ||
                avcType->bEntropyCodingCABAC != OMX_FALSE ||
                avcType->bconstIpred != OMX_FALSE ||
                avcType->bDirect8x8Inference != OMX_FALSE ||
                avcType->bDirectSpatialTemporal != OMX_FALSE ||
                avcType->nCabacInitIdc != 0) {
 return OMX_ErrorUndefined;
 }

 if (OK != ConvertOmxAvcLevelToAvcSpecLevel(avcType->eLevel, &mAVCEncLevel)) {
 return OMX_ErrorUndefined;
 }

 return OMX_ErrorNone;
 }

 default:
 return SoftVideoEncoderOMXComponent::internalSetParameter(index, params);
 }
}
