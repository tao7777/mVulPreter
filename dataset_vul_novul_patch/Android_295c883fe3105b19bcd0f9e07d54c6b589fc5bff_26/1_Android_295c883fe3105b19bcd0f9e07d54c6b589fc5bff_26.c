OMX_ERRORTYPE SoftMP3::internalSetParameter(
        OMX_INDEXTYPE index, const OMX_PTR params) {
 switch (index) {
 case OMX_IndexParamStandardComponentRole:
 {

             const OMX_PARAM_COMPONENTROLETYPE *roleParams =
                 (const OMX_PARAM_COMPONENTROLETYPE *)params;
 
             if (strncmp((const char *)roleParams->cRole,
                         "audio_decoder.mp3",
                         OMX_MAX_STRINGNAME_SIZE - 1)) {
 return OMX_ErrorUndefined;
 }

 return OMX_ErrorNone;
 }

 case OMX_IndexParamAudioPcm:
 {

             const OMX_AUDIO_PARAM_PCMMODETYPE *pcmParams =
                 (const OMX_AUDIO_PARAM_PCMMODETYPE *)params;
 
             if (pcmParams->nPortIndex != 1) {
                 return OMX_ErrorUndefined;
             }

            mNumChannels = pcmParams->nChannels;
            mSamplingRate = pcmParams->nSamplingRate;

 return OMX_ErrorNone;
 }

 default:
 return SimpleSoftOMXComponent::internalSetParameter(index, params);
 }
}
