OMX_ERRORTYPE SoftVorbis::internalSetParameter(
        OMX_INDEXTYPE index, const OMX_PTR params) {
 switch (index) {
 case OMX_IndexParamStandardComponentRole:
 {

             const OMX_PARAM_COMPONENTROLETYPE *roleParams =
                 (const OMX_PARAM_COMPONENTROLETYPE *)params;
 
            if (!isValidOMXParam(roleParams)) {
                return OMX_ErrorBadParameter;
            }

             if (strncmp((const char *)roleParams->cRole,
                         "audio_decoder.vorbis",
                         OMX_MAX_STRINGNAME_SIZE - 1)) {
 return OMX_ErrorUndefined;
 }

 return OMX_ErrorNone;
 }

 case OMX_IndexParamAudioVorbis:
 {

             const OMX_AUDIO_PARAM_VORBISTYPE *vorbisParams =
                 (const OMX_AUDIO_PARAM_VORBISTYPE *)params;
 
            if (!isValidOMXParam(vorbisParams)) {
                return OMX_ErrorBadParameter;
            }

             if (vorbisParams->nPortIndex != 0) {
                 return OMX_ErrorUndefined;
             }

 return OMX_ErrorNone;
 }

 default:
 return SimpleSoftOMXComponent::internalSetParameter(index, params);
 }
}
