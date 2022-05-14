OMX_ERRORTYPE SoftAMRNBEncoder::internalGetParameter(
        OMX_INDEXTYPE index, OMX_PTR params) {
 switch (index) {
 case OMX_IndexParamAudioPortFormat:
 {

             OMX_AUDIO_PARAM_PORTFORMATTYPE *formatParams =
                 (OMX_AUDIO_PARAM_PORTFORMATTYPE *)params;
 
            if (!isValidOMXParam(formatParams)) {
                return OMX_ErrorBadParameter;
            }

             if (formatParams->nPortIndex > 1) {
                 return OMX_ErrorUndefined;
             }

 if (formatParams->nIndex > 0) {
 return OMX_ErrorNoMore;
 }

            formatParams->eEncoding =
 (formatParams->nPortIndex == 0)
 ? OMX_AUDIO_CodingPCM : OMX_AUDIO_CodingAMR;

 return OMX_ErrorNone;
 }

 case OMX_IndexParamAudioAmr:
 {

             OMX_AUDIO_PARAM_AMRTYPE *amrParams =
                 (OMX_AUDIO_PARAM_AMRTYPE *)params;
 
            if (!isValidOMXParam(amrParams)) {
                return OMX_ErrorBadParameter;
            }

             if (amrParams->nPortIndex != 1) {
                 return OMX_ErrorUndefined;
             }

            amrParams->nChannels = 1;
            amrParams->nBitRate = mBitRate;
            amrParams->eAMRBandMode = (OMX_AUDIO_AMRBANDMODETYPE)(mMode + 1);
            amrParams->eAMRDTXMode = OMX_AUDIO_AMRDTXModeOff;
            amrParams->eAMRFrameFormat = OMX_AUDIO_AMRFrameFormatFSF;

 return OMX_ErrorNone;
 }

 case OMX_IndexParamAudioPcm:
 {

             OMX_AUDIO_PARAM_PCMMODETYPE *pcmParams =
                 (OMX_AUDIO_PARAM_PCMMODETYPE *)params;
 
            if (!isValidOMXParam(pcmParams)) {
                return OMX_ErrorBadParameter;
            }

             if (pcmParams->nPortIndex != 0) {
                 return OMX_ErrorUndefined;
             }

            pcmParams->eNumData = OMX_NumericalDataSigned;
            pcmParams->eEndian = OMX_EndianBig;
            pcmParams->bInterleaved = OMX_TRUE;
            pcmParams->nBitPerSample = 16;
            pcmParams->ePCMMode = OMX_AUDIO_PCMModeLinear;
            pcmParams->eChannelMapping[0] = OMX_AUDIO_ChannelCF;

            pcmParams->nChannels = 1;
            pcmParams->nSamplingRate = kSampleRate;

 return OMX_ErrorNone;
 }

 default:
 return SimpleSoftOMXComponent::internalGetParameter(index, params);
 }
}
