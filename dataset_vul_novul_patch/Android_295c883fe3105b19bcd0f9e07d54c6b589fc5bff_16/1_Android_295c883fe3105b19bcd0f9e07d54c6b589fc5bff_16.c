OMX_ERRORTYPE SoftAVC::setConfig(
        OMX_INDEXTYPE index, const OMX_PTR _params) {
 switch (index) {
 case OMX_IndexConfigVideoIntraVOPRefresh:
 {

             OMX_CONFIG_INTRAREFRESHVOPTYPE *params =
                 (OMX_CONFIG_INTRAREFRESHVOPTYPE *)_params;
 
             if (params->nPortIndex != kOutputPortIndex) {
                 return OMX_ErrorBadPortIndex;
             }

            mKeyFrameRequested = params->IntraRefreshVOP;
 return OMX_ErrorNone;
 }

 case OMX_IndexConfigVideoBitrate:
 {

             OMX_VIDEO_CONFIG_BITRATETYPE *params =
                 (OMX_VIDEO_CONFIG_BITRATETYPE *)_params;
 
             if (params->nPortIndex != kOutputPortIndex) {
                 return OMX_ErrorBadPortIndex;
             }

 if (mBitrate != params->nEncodeBitrate) {
                mBitrate = params->nEncodeBitrate;
                mBitrateUpdated = true;
 }
 return OMX_ErrorNone;
 }

 default:
 return SimpleSoftOMXComponent::setConfig(index, _params);
 }
}
