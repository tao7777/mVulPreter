OMX_ERRORTYPE SoftOpus::internalGetParameter(
        OMX_INDEXTYPE index, OMX_PTR params) {
 switch ((int)index) {
 case OMX_IndexParamAudioAndroidOpus:
 {

             OMX_AUDIO_PARAM_ANDROID_OPUSTYPE *opusParams =
                 (OMX_AUDIO_PARAM_ANDROID_OPUSTYPE *)params;
 
            if (!isValidOMXParam(opusParams)) {
                return OMX_ErrorBadParameter;
            }

             if (opusParams->nPortIndex != 0) {
                 return OMX_ErrorUndefined;
             }

            opusParams->nAudioBandWidth = 0;
            opusParams->nSampleRate = kRate;
            opusParams->nBitRate = 0;

 if (!isConfigured()) {
                opusParams->nChannels = 1;
 } else {
                opusParams->nChannels = mHeader->channels;
 }

 return OMX_ErrorNone;
 }

 case OMX_IndexParamAudioPcm:
 {

             OMX_AUDIO_PARAM_PCMMODETYPE *pcmParams =
                 (OMX_AUDIO_PARAM_PCMMODETYPE *)params;
 
            if (!isValidOMXParam(pcmParams)) {
                return OMX_ErrorBadParameter;
            }

             if (pcmParams->nPortIndex != 1) {
                 return OMX_ErrorUndefined;
             }

            pcmParams->eNumData = OMX_NumericalDataSigned;
            pcmParams->eEndian = OMX_EndianBig;
            pcmParams->bInterleaved = OMX_TRUE;
            pcmParams->nBitPerSample = 16;
            pcmParams->ePCMMode = OMX_AUDIO_PCMModeLinear;
            pcmParams->eChannelMapping[0] = OMX_AUDIO_ChannelLF;
            pcmParams->eChannelMapping[1] = OMX_AUDIO_ChannelRF;
            pcmParams->nSamplingRate = kRate;

 if (!isConfigured()) {
                pcmParams->nChannels = 1;
 } else {
                pcmParams->nChannels = mHeader->channels;
 }

 return OMX_ErrorNone;
 }

 default:
 return SimpleSoftOMXComponent::internalGetParameter(index, params);
 }
}
