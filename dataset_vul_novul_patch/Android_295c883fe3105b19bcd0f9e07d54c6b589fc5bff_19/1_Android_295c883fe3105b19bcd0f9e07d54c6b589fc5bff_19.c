OMX_ERRORTYPE SoftG711::internalGetParameter(
        OMX_INDEXTYPE index, OMX_PTR params) {
 switch (index) {
 case OMX_IndexParamAudioPcm:
 {

             OMX_AUDIO_PARAM_PCMMODETYPE *pcmParams =
                 (OMX_AUDIO_PARAM_PCMMODETYPE *)params;
 
             if (pcmParams->nPortIndex > 1) {
                 return OMX_ErrorUndefined;
             }

            pcmParams->eNumData = OMX_NumericalDataSigned;
            pcmParams->eEndian = OMX_EndianBig;
            pcmParams->bInterleaved = OMX_TRUE;
            pcmParams->nBitPerSample = 16;
 if (pcmParams->nPortIndex == 0) {
                pcmParams->ePCMMode = mIsMLaw ? OMX_AUDIO_PCMModeMULaw
 : OMX_AUDIO_PCMModeALaw;
 } else {
                pcmParams->ePCMMode = OMX_AUDIO_PCMModeLinear;
 }
            pcmParams->eChannelMapping[0] = OMX_AUDIO_ChannelLF;
            pcmParams->eChannelMapping[1] = OMX_AUDIO_ChannelRF;

            pcmParams->nChannels = mNumChannels;
            pcmParams->nSamplingRate = mSamplingRate;

 return OMX_ErrorNone;
 }

 default:
 return SimpleSoftOMXComponent::internalGetParameter(index, params);
 }
}
