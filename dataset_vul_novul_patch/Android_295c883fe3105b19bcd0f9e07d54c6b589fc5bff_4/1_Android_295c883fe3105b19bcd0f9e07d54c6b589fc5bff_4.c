OMX_ERRORTYPE SoftAACEncoder2::internalGetParameter(
        OMX_INDEXTYPE index, OMX_PTR params) {
 switch (index) {
 case OMX_IndexParamAudioPortFormat:
 {

             OMX_AUDIO_PARAM_PORTFORMATTYPE *formatParams =
                 (OMX_AUDIO_PARAM_PORTFORMATTYPE *)params;
 
             if (formatParams->nPortIndex > 1) {
                 return OMX_ErrorUndefined;
             }

 if (formatParams->nIndex > 0) {
 return OMX_ErrorNoMore;
 }

            formatParams->eEncoding =
 (formatParams->nPortIndex == 0)
 ? OMX_AUDIO_CodingPCM : OMX_AUDIO_CodingAAC;

 return OMX_ErrorNone;
 }

 case OMX_IndexParamAudioAac:
 {

             OMX_AUDIO_PARAM_AACPROFILETYPE *aacParams =
                 (OMX_AUDIO_PARAM_AACPROFILETYPE *)params;
 
             if (aacParams->nPortIndex != 1) {
                 return OMX_ErrorUndefined;
             }

            aacParams->nBitRate = mBitRate;
            aacParams->nAudioBandWidth = 0;
            aacParams->nAACtools = 0;
            aacParams->nAACERtools = 0;
            aacParams->eAACProfile = (OMX_AUDIO_AACPROFILETYPE) mAACProfile;
            aacParams->eAACStreamFormat = OMX_AUDIO_AACStreamFormatMP4FF;
            aacParams->eChannelMode = OMX_AUDIO_ChannelModeStereo;

            aacParams->nChannels = mNumChannels;
            aacParams->nSampleRate = mSampleRate;
            aacParams->nFrameLength = 0;

 switch (mSBRMode) {
 case 1: // sbr on
 switch (mSBRRatio) {
 case 0:
                    aacParams->nAACtools |= OMX_AUDIO_AACToolAndroidSSBR;
                    aacParams->nAACtools |= OMX_AUDIO_AACToolAndroidDSBR;
 break;
 case 1:
                    aacParams->nAACtools |= OMX_AUDIO_AACToolAndroidSSBR;
                    aacParams->nAACtools &= ~OMX_AUDIO_AACToolAndroidDSBR;
 break;
 case 2:
                    aacParams->nAACtools &= ~OMX_AUDIO_AACToolAndroidSSBR;
                    aacParams->nAACtools |= OMX_AUDIO_AACToolAndroidDSBR;
 break;
 default:
                    ALOGE("invalid SBR ratio %d", mSBRRatio);
                    TRESPASS();
 }
 break;
 case 0: // sbr off
 case -1: // sbr undefined
                aacParams->nAACtools &= ~OMX_AUDIO_AACToolAndroidSSBR;
                aacParams->nAACtools &= ~OMX_AUDIO_AACToolAndroidDSBR;
 break;
 default:
                ALOGE("invalid SBR mode %d", mSBRMode);
                TRESPASS();
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

            pcmParams->eNumData = OMX_NumericalDataSigned;
            pcmParams->eEndian = OMX_EndianBig;
            pcmParams->bInterleaved = OMX_TRUE;
            pcmParams->nBitPerSample = 16;
            pcmParams->ePCMMode = OMX_AUDIO_PCMModeLinear;
            pcmParams->eChannelMapping[0] = OMX_AUDIO_ChannelLF;
            pcmParams->eChannelMapping[1] = OMX_AUDIO_ChannelRF;

            pcmParams->nChannels = mNumChannels;
            pcmParams->nSamplingRate = mSampleRate;

 return OMX_ErrorNone;
 }

 default:
 return SimpleSoftOMXComponent::internalGetParameter(index, params);
 }
}
