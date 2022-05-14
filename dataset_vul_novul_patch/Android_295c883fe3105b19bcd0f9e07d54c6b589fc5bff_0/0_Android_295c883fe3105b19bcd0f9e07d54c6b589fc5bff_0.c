OMX_ERRORTYPE SoftAAC2::internalGetParameter(
        OMX_INDEXTYPE index, OMX_PTR params) {
 switch (index) {
 case OMX_IndexParamAudioAac:
 {

             OMX_AUDIO_PARAM_AACPROFILETYPE *aacParams =
                 (OMX_AUDIO_PARAM_AACPROFILETYPE *)params;
 
            if (!isValidOMXParam(aacParams)) {
                return OMX_ErrorBadParameter;
            }

             if (aacParams->nPortIndex != 0) {
                 return OMX_ErrorUndefined;
             }

            aacParams->nBitRate = 0;
            aacParams->nAudioBandWidth = 0;
            aacParams->nAACtools = 0;
            aacParams->nAACERtools = 0;
            aacParams->eAACProfile = OMX_AUDIO_AACObjectMain;

            aacParams->eAACStreamFormat =
                mIsADTS
 ? OMX_AUDIO_AACStreamFormatMP4ADTS
 : OMX_AUDIO_AACStreamFormatMP4FF;

            aacParams->eChannelMode = OMX_AUDIO_ChannelModeStereo;

 if (!isConfigured()) {
                aacParams->nChannels = 1;
                aacParams->nSampleRate = 44100;
                aacParams->nFrameLength = 0;
 } else {
                aacParams->nChannels = mStreamInfo->numChannels;
                aacParams->nSampleRate = mStreamInfo->sampleRate;
                aacParams->nFrameLength = mStreamInfo->frameSize;
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
            pcmParams->eChannelMapping[2] = OMX_AUDIO_ChannelCF;
            pcmParams->eChannelMapping[3] = OMX_AUDIO_ChannelLFE;
            pcmParams->eChannelMapping[4] = OMX_AUDIO_ChannelLS;
            pcmParams->eChannelMapping[5] = OMX_AUDIO_ChannelRS;

 if (!isConfigured()) {
                pcmParams->nChannels = 1;
                pcmParams->nSamplingRate = 44100;
 } else {
                pcmParams->nChannels = mStreamInfo->numChannels;
                pcmParams->nSamplingRate = mStreamInfo->sampleRate;
 }

 return OMX_ErrorNone;
 }

 default:
 return SimpleSoftOMXComponent::internalGetParameter(index, params);
 }
}
