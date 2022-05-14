OMX_ERRORTYPE SoftAACEncoder2::internalSetParameter(
        OMX_INDEXTYPE index, const OMX_PTR params) {
 switch (index) {
 case OMX_IndexParamStandardComponentRole:
 {

             const OMX_PARAM_COMPONENTROLETYPE *roleParams =
                 (const OMX_PARAM_COMPONENTROLETYPE *)params;
 
             if (strncmp((const char *)roleParams->cRole,
                         "audio_encoder.aac",
                         OMX_MAX_STRINGNAME_SIZE - 1)) {
 return OMX_ErrorUndefined;
 }

 return OMX_ErrorNone;
 }

 case OMX_IndexParamAudioPortFormat:
 {

             const OMX_AUDIO_PARAM_PORTFORMATTYPE *formatParams =
                 (const OMX_AUDIO_PARAM_PORTFORMATTYPE *)params;
 
             if (formatParams->nPortIndex > 1) {
                 return OMX_ErrorUndefined;
             }

 if (formatParams->nIndex > 0) {
 return OMX_ErrorNoMore;
 }

 if ((formatParams->nPortIndex == 0
 && formatParams->eEncoding != OMX_AUDIO_CodingPCM)
 || (formatParams->nPortIndex == 1
 && formatParams->eEncoding != OMX_AUDIO_CodingAAC)) {
 return OMX_ErrorUndefined;
 }

 return OMX_ErrorNone;
 }

 case OMX_IndexParamAudioAac:
 {

             OMX_AUDIO_PARAM_AACPROFILETYPE *aacParams =
                 (OMX_AUDIO_PARAM_AACPROFILETYPE *)params;
 
             if (aacParams->nPortIndex != 1) {
                 return OMX_ErrorUndefined;
             }

            mBitRate = aacParams->nBitRate;
            mNumChannels = aacParams->nChannels;
            mSampleRate = aacParams->nSampleRate;
 if (aacParams->eAACProfile != OMX_AUDIO_AACObjectNull) {
                mAACProfile = aacParams->eAACProfile;
 }

 if (!(aacParams->nAACtools & OMX_AUDIO_AACToolAndroidSSBR)
 && !(aacParams->nAACtools & OMX_AUDIO_AACToolAndroidDSBR)) {
                mSBRMode = 0;
                mSBRRatio = 0;
 } else if ((aacParams->nAACtools & OMX_AUDIO_AACToolAndroidSSBR)
 && !(aacParams->nAACtools & OMX_AUDIO_AACToolAndroidDSBR)) {
                mSBRMode = 1;
                mSBRRatio = 1;
 } else if (!(aacParams->nAACtools & OMX_AUDIO_AACToolAndroidSSBR)
 && (aacParams->nAACtools & OMX_AUDIO_AACToolAndroidDSBR)) {
                mSBRMode = 1;
                mSBRRatio = 2;
 } else {
                mSBRMode = -1; // codec default sbr mode
                mSBRRatio = 0;
 }

 if (setAudioParams() != OK) {
 return OMX_ErrorUndefined;
 }

 return OMX_ErrorNone;
 }

 case OMX_IndexParamAudioPcm:
 {

             OMX_AUDIO_PARAM_PCMMODETYPE *pcmParams =
                 (OMX_AUDIO_PARAM_PCMMODETYPE *)params;
 
             if (pcmParams->nPortIndex != 0) {
                 return OMX_ErrorUndefined;
             }

            mNumChannels = pcmParams->nChannels;
            mSampleRate = pcmParams->nSamplingRate;
 if (setAudioParams() != OK) {
 return OMX_ErrorUndefined;
 }

 return OMX_ErrorNone;
 }

 default:
 return SimpleSoftOMXComponent::internalSetParameter(index, params);
 }
}
