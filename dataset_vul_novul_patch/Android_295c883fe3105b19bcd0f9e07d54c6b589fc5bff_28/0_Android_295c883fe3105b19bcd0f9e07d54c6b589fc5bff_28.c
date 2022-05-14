OMX_ERRORTYPE SoftVPXEncoder::internalSetParameter(OMX_INDEXTYPE index,
 const OMX_PTR param) {

     const int32_t indexFull = index;
 
     switch (indexFull) {
        case OMX_IndexParamVideoBitrate: {
            const OMX_VIDEO_PARAM_BITRATETYPE *bitRate =
                (const OMX_VIDEO_PARAM_BITRATETYPE*) param;
 
            if (!isValidOMXParam(bitRate)) {
                return OMX_ErrorBadParameter;
            }
 
            return internalSetBitrateParams(bitRate);
        }

        case OMX_IndexParamVideoVp8: {
            const OMX_VIDEO_PARAM_VP8TYPE *vp8Params =
                (const OMX_VIDEO_PARAM_VP8TYPE*) param;

            if (!isValidOMXParam(vp8Params)) {
                return OMX_ErrorBadParameter;
            }

            return internalSetVp8Params(vp8Params);
        }

        case OMX_IndexParamVideoAndroidVp8Encoder: {
            const OMX_VIDEO_PARAM_ANDROID_VP8ENCODERTYPE *vp8AndroidParams =
                (const OMX_VIDEO_PARAM_ANDROID_VP8ENCODERTYPE*) param;

            if (!isValidOMXParam(vp8AndroidParams)) {
                return OMX_ErrorBadParameter;
            }

            return internalSetAndroidVp8Params(vp8AndroidParams);
        }
 
         default:
             return SoftVideoEncoderOMXComponent::internalSetParameter(index, param);
 }
}
