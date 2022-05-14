OMX_ERRORTYPE SoftVPXEncoder::internalSetParameter(OMX_INDEXTYPE index,
 const OMX_PTR param) {

     const int32_t indexFull = index;
 
     switch (indexFull) {
        case OMX_IndexParamVideoBitrate:
            return internalSetBitrateParams(
                (const OMX_VIDEO_PARAM_BITRATETYPE *)param);
 
        case OMX_IndexParamVideoVp8:
            return internalSetVp8Params(
                (const OMX_VIDEO_PARAM_VP8TYPE *)param);
 
        case OMX_IndexParamVideoAndroidVp8Encoder:
            return internalSetAndroidVp8Params(
                (const OMX_VIDEO_PARAM_ANDROID_VP8ENCODERTYPE *)param);
 
         default:
             return SoftVideoEncoderOMXComponent::internalSetParameter(index, param);
 }
}
