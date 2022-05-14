OMX_ERRORTYPE SoftG711::internalSetParameter(
        OMX_INDEXTYPE index, const OMX_PTR params) {
 switch (index) {
 case OMX_IndexParamAudioPcm:
 {

             OMX_AUDIO_PARAM_PCMMODETYPE *pcmParams =
                 (OMX_AUDIO_PARAM_PCMMODETYPE *)params;
 
             if (pcmParams->nPortIndex != 0 && pcmParams->nPortIndex != 1) {
                 return OMX_ErrorUndefined;
             }

 if (pcmParams->nChannels < 1 || pcmParams->nChannels > 2) {
 return OMX_ErrorUndefined;
 }

 if(pcmParams->nPortIndex == 0) {
                mNumChannels = pcmParams->nChannels;
 }

            mSamplingRate = pcmParams->nSamplingRate;

 return OMX_ErrorNone;
 }

 case OMX_IndexParamStandardComponentRole:
 {

             const OMX_PARAM_COMPONENTROLETYPE *roleParams =
                 (const OMX_PARAM_COMPONENTROLETYPE *)params;
 
             if (mIsMLaw) {
                 if (strncmp((const char *)roleParams->cRole,
                             "audio_decoder.g711mlaw",
                            OMX_MAX_STRINGNAME_SIZE - 1)) {
 return OMX_ErrorUndefined;
 }
 } else {
 if (strncmp((const char *)roleParams->cRole,
 "audio_decoder.g711alaw",
                            OMX_MAX_STRINGNAME_SIZE - 1)) {
 return OMX_ErrorUndefined;
 }
 }

 return OMX_ErrorNone;
 }

 default:
 return SimpleSoftOMXComponent::internalSetParameter(index, params);
 }
}
