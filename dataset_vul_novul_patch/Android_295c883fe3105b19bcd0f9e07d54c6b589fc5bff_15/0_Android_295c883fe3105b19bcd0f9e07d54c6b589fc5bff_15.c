OMX_ERRORTYPE SoftAVC::internalSetParameter(OMX_INDEXTYPE index, const OMX_PTR params) {
 int32_t indexFull = index;


     switch (indexFull) {
         case OMX_IndexParamVideoBitrate:
         {
            OMX_VIDEO_PARAM_BITRATETYPE *bitRate =
                (OMX_VIDEO_PARAM_BITRATETYPE *)params;

            if (!isValidOMXParam(bitRate)) {
                return OMX_ErrorBadParameter;
            }

            return internalSetBitrateParams(bitRate);
         }
 
         case OMX_IndexParamVideoAvc:
         {
             OMX_VIDEO_PARAM_AVCTYPE *avcType = (OMX_VIDEO_PARAM_AVCTYPE *)params;
 
            if (!isValidOMXParam(avcType)) {
                return OMX_ErrorBadParameter;
            }

             if (avcType->nPortIndex != 1) {
                 return OMX_ErrorUndefined;
             }

            mEntropyMode = 0;

 if (OMX_TRUE == avcType->bEntropyCodingCABAC)
                mEntropyMode = 1;

 if ((avcType->nAllowedPictureTypes & OMX_VIDEO_PictureTypeB) &&
                    avcType->nPFrames) {
                mBframes = avcType->nBFrames / avcType->nPFrames;
 }

            mIInterval = avcType->nPFrames + avcType->nBFrames;

 if (OMX_VIDEO_AVCLoopFilterDisable == avcType->eLoopFilterMode)
                mDisableDeblkLevel = 4;

 if (avcType->nRefFrames != 1
 || avcType->bUseHadamard != OMX_TRUE
 || avcType->nRefIdx10ActiveMinus1 != 0
 || avcType->nRefIdx11ActiveMinus1 != 0
 || avcType->bWeightedPPrediction != OMX_FALSE
 || avcType->bconstIpred != OMX_FALSE
 || avcType->bDirect8x8Inference != OMX_FALSE
 || avcType->bDirectSpatialTemporal != OMX_FALSE
 || avcType->nCabacInitIdc != 0) {
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
