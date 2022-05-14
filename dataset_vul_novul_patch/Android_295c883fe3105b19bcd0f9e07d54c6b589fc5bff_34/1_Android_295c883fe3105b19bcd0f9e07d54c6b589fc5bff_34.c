OMX_ERRORTYPE SoftVorbis::internalGetParameter(
        OMX_INDEXTYPE index, OMX_PTR params) {
 switch (index) {
 case OMX_IndexParamAudioVorbis:
 {

             OMX_AUDIO_PARAM_VORBISTYPE *vorbisParams =
                 (OMX_AUDIO_PARAM_VORBISTYPE *)params;
 
             if (vorbisParams->nPortIndex != 0) {
                 return OMX_ErrorUndefined;
             }

            vorbisParams->nBitRate = 0;
            vorbisParams->nMinBitRate = 0;
            vorbisParams->nMaxBitRate = 0;
            vorbisParams->nAudioBandWidth = 0;
            vorbisParams->nQuality = 3;
            vorbisParams->bManaged = OMX_FALSE;
            vorbisParams->bDownmix = OMX_FALSE;

 if (!isConfigured()) {
                vorbisParams->nChannels = 1;
                vorbisParams->nSampleRate = 44100;
 } else {
                vorbisParams->nChannels = mVi->channels;
                vorbisParams->nSampleRate = mVi->rate;
                vorbisParams->nBitRate = mVi->bitrate_nominal;
                vorbisParams->nMinBitRate = mVi->bitrate_lower;
                vorbisParams->nMaxBitRate = mVi->bitrate_upper;
 }

 return OMX_ErrorNone;
 }

 case OMX_IndexParamAudioPcm:
 {

             OMX_AUDIO_PARAM_PCMMODETYPE *pcmParams =
                 (OMX_AUDIO_PARAM_PCMMODETYPE *)params;
 
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

 if (!isConfigured()) {
                pcmParams->nChannels = 1;
                pcmParams->nSamplingRate = 44100;
 } else {
                pcmParams->nChannels = mVi->channels;
                pcmParams->nSamplingRate = mVi->rate;
 }

 return OMX_ErrorNone;
 }

 default:
 return SimpleSoftOMXComponent::internalGetParameter(index, params);
 }
}
