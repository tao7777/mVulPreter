OMX_ERRORTYPE SoftRaw::internalGetParameter(
        OMX_INDEXTYPE index, OMX_PTR params) {
 switch (index) {
 case OMX_IndexParamAudioPcm:
 {

             OMX_AUDIO_PARAM_PCMMODETYPE *pcmParams =
                 (OMX_AUDIO_PARAM_PCMMODETYPE *)params;
 
             if (pcmParams->nPortIndex != 0 && pcmParams->nPortIndex != 1) {
                 return OMX_ErrorUndefined;
             }

            pcmParams->eNumData = OMX_NumericalDataSigned;
            pcmParams->eEndian = OMX_EndianBig;
            pcmParams->bInterleaved = OMX_TRUE;
            pcmParams->nBitPerSample = 16;
            pcmParams->ePCMMode = OMX_AUDIO_PCMModeLinear;
            pcmParams->eChannelMapping[0] = OMX_AUDIO_ChannelLF;
            pcmParams->eChannelMapping[1] = OMX_AUDIO_ChannelRF;

            pcmParams->nChannels = mChannelCount;
            pcmParams->nSamplingRate = mSampleRate;

 return OMX_ErrorNone;
 }

 default:
 return SimpleSoftOMXComponent::internalGetParameter(index, params);
 }
}
