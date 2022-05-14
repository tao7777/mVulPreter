OMX_ERRORTYPE SoftMP3::internalGetParameter(
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
            pcmParams->ePCMMode = OMX_AUDIO_PCMModeLinear;
            pcmParams->eChannelMapping[0] = OMX_AUDIO_ChannelLF;
            pcmParams->eChannelMapping[1] = OMX_AUDIO_ChannelRF;

            pcmParams->nChannels = mNumChannels;
            pcmParams->nSamplingRate = mSamplingRate;

 return OMX_ErrorNone;
 }

 case OMX_IndexParamAudioMp3:
 {

             OMX_AUDIO_PARAM_MP3TYPE *mp3Params =
                 (OMX_AUDIO_PARAM_MP3TYPE *)params;
 
             if (mp3Params->nPortIndex > 1) {
                 return OMX_ErrorUndefined;
             }

            mp3Params->nChannels = mNumChannels;
            mp3Params->nBitRate = 0 /* unknown */;
            mp3Params->nSampleRate = mSamplingRate;

 return OMX_ErrorNone;
 }

 default:
 return SimpleSoftOMXComponent::internalGetParameter(index, params);
 }
}
