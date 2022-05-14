OMX_ERRORTYPE SoftAMR::internalSetParameter(
        OMX_INDEXTYPE index, const OMX_PTR params) {
 switch (index) {
 case OMX_IndexParamStandardComponentRole:
 {

             const OMX_PARAM_COMPONENTROLETYPE *roleParams =
                 (const OMX_PARAM_COMPONENTROLETYPE *)params;
 
            if (!isValidOMXParam(roleParams)) {
                return OMX_ErrorBadParameter;
            }

             if (mMode == MODE_NARROW) {
                 if (strncmp((const char *)roleParams->cRole,
                             "audio_decoder.amrnb",
                            OMX_MAX_STRINGNAME_SIZE - 1)) {
 return OMX_ErrorUndefined;
 }
 } else {
 if (strncmp((const char *)roleParams->cRole,
 "audio_decoder.amrwb",
                            OMX_MAX_STRINGNAME_SIZE - 1)) {
 return OMX_ErrorUndefined;
 }
 }

 return OMX_ErrorNone;
 }

 case OMX_IndexParamAudioAmr:
 {

             const OMX_AUDIO_PARAM_AMRTYPE *aacParams =
                 (const OMX_AUDIO_PARAM_AMRTYPE *)params;
 
            if (!isValidOMXParam(aacParams)) {
                return OMX_ErrorBadParameter;
            }

             if (aacParams->nPortIndex != 0) {
                 return OMX_ErrorUndefined;
             }

 return OMX_ErrorNone;
 }

 case OMX_IndexParamAudioPcm:
 {

             const OMX_AUDIO_PARAM_PCMMODETYPE *pcmParams =
                 (OMX_AUDIO_PARAM_PCMMODETYPE *)params;
 
            if (!isValidOMXParam(pcmParams)) {
                return OMX_ErrorBadParameter;
            }

             if (pcmParams->nPortIndex != 1) {
                 return OMX_ErrorUndefined;
             }

 return OMX_ErrorNone;
 }

 default:
 return SimpleSoftOMXComponent::internalSetParameter(index, params);
 }
}
