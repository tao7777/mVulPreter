OMX_ERRORTYPE SoftOpus::internalSetParameter(
        OMX_INDEXTYPE index, const OMX_PTR params) {
 switch ((int)index) {
 case OMX_IndexParamStandardComponentRole:
 {

             const OMX_PARAM_COMPONENTROLETYPE *roleParams =
                 (const OMX_PARAM_COMPONENTROLETYPE *)params;
 
            if (!isValidOMXParam(roleParams)) {
                return OMX_ErrorBadParameter;
            }

             if (strncmp((const char *)roleParams->cRole,
                         "audio_decoder.opus",
                         OMX_MAX_STRINGNAME_SIZE - 1)) {
 return OMX_ErrorUndefined;
 }

 return OMX_ErrorNone;
 }

 case OMX_IndexParamAudioAndroidOpus:
 {

             const OMX_AUDIO_PARAM_ANDROID_OPUSTYPE *opusParams =
                 (const OMX_AUDIO_PARAM_ANDROID_OPUSTYPE *)params;
 
            if (!isValidOMXParam(opusParams)) {
                return OMX_ErrorBadParameter;
            }

             if (opusParams->nPortIndex != 0) {
                 return OMX_ErrorUndefined;
             }

 return OMX_ErrorNone;
 }

 default:
 return SimpleSoftOMXComponent::internalSetParameter(index, params);
 }
}
