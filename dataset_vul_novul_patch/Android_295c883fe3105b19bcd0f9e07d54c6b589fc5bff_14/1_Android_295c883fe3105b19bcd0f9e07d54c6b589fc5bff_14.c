OMX_ERRORTYPE SoftAVC::internalGetParameter(OMX_INDEXTYPE index, OMX_PTR params) {
 switch (index) {
 case OMX_IndexParamVideoBitrate:
 {

             OMX_VIDEO_PARAM_BITRATETYPE *bitRate =
                 (OMX_VIDEO_PARAM_BITRATETYPE *)params;
 
             if (bitRate->nPortIndex != 1) {
                 return OMX_ErrorUndefined;
             }

            bitRate->eControlRate = OMX_Video_ControlRateVariable;
            bitRate->nTargetBitrate = mBitrate;
 return OMX_ErrorNone;
 }

 case OMX_IndexParamVideoAvc:

         {
             OMX_VIDEO_PARAM_AVCTYPE *avcParams = (OMX_VIDEO_PARAM_AVCTYPE *)params;
 
             if (avcParams->nPortIndex != 1) {
                 return OMX_ErrorUndefined;
             }

            OMX_VIDEO_AVCLEVELTYPE omxLevel = OMX_VIDEO_AVCLevel41;
 if (OMX_ErrorNone
 != ConvertAvcSpecLevelToOmxAvcLevel(mAVCEncLevel, &omxLevel)) {
 return OMX_ErrorUndefined;
 }

            avcParams->eProfile = OMX_VIDEO_AVCProfileBaseline;
            avcParams->eLevel = omxLevel;
            avcParams->nRefFrames = 1;
            avcParams->bUseHadamard = OMX_TRUE;
            avcParams->nAllowedPictureTypes = (OMX_VIDEO_PictureTypeI
 | OMX_VIDEO_PictureTypeP | OMX_VIDEO_PictureTypeB);
            avcParams->nRefIdx10ActiveMinus1 = 0;
            avcParams->nRefIdx11ActiveMinus1 = 0;
            avcParams->bWeightedPPrediction = OMX_FALSE;
            avcParams->bconstIpred = OMX_FALSE;
            avcParams->bDirect8x8Inference = OMX_FALSE;
            avcParams->bDirectSpatialTemporal = OMX_FALSE;
            avcParams->nCabacInitIdc = 0;
 return OMX_ErrorNone;
 }

 default:
 return SoftVideoEncoderOMXComponent::internalGetParameter(index, params);
 }
}
