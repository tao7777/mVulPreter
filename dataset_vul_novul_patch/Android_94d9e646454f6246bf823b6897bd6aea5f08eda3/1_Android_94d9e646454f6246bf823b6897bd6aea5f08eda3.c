status_t ACodec::setupAACCodec(
 bool encoder, int32_t numChannels, int32_t sampleRate,
 int32_t bitRate, int32_t aacProfile, bool isADTS, int32_t sbrMode,
 int32_t maxOutputChannelCount, const drcParams_t& drc,
 int32_t pcmLimiterEnable) {
 if (encoder && isADTS) {
 return -EINVAL;
 }

 status_t err = setupRawAudioFormat(
            encoder ? kPortIndexInput : kPortIndexOutput,
            sampleRate,
            numChannels);

 if (err != OK) {
 return err;
 }

 if (encoder) {
        err = selectAudioPortFormat(kPortIndexOutput, OMX_AUDIO_CodingAAC);

 if (err != OK) {
 return err;
 }

        OMX_PARAM_PORTDEFINITIONTYPE def;
 InitOMXParams(&def);
        def.nPortIndex = kPortIndexOutput;

        err = mOMX->getParameter(
                mNode, OMX_IndexParamPortDefinition, &def, sizeof(def));

 if (err != OK) {
 return err;
 }

        def.format.audio.bFlagErrorConcealment = OMX_TRUE;
        def.format.audio.eEncoding = OMX_AUDIO_CodingAAC;

        err = mOMX->setParameter(
                mNode, OMX_IndexParamPortDefinition, &def, sizeof(def));

 if (err != OK) {
 return err;
 }

        OMX_AUDIO_PARAM_AACPROFILETYPE profile;
 InitOMXParams(&profile);
        profile.nPortIndex = kPortIndexOutput;

        err = mOMX->getParameter(
                mNode, OMX_IndexParamAudioAac, &profile, sizeof(profile));

 if (err != OK) {
 return err;
 }

        profile.nChannels = numChannels;

        profile.eChannelMode =
 (numChannels == 1)
 ? OMX_AUDIO_ChannelModeMono: OMX_AUDIO_ChannelModeStereo;

        profile.nSampleRate = sampleRate;
        profile.nBitRate = bitRate;
        profile.nAudioBandWidth = 0;
        profile.nFrameLength = 0;
        profile.nAACtools = OMX_AUDIO_AACToolAll;
        profile.nAACERtools = OMX_AUDIO_AACERNone;
        profile.eAACProfile = (OMX_AUDIO_AACPROFILETYPE) aacProfile;
        profile.eAACStreamFormat = OMX_AUDIO_AACStreamFormatMP4FF;
 switch (sbrMode) {
 case 0:
            profile.nAACtools &= ~OMX_AUDIO_AACToolAndroidSSBR;
            profile.nAACtools &= ~OMX_AUDIO_AACToolAndroidDSBR;
 break;
 case 1:
            profile.nAACtools |= OMX_AUDIO_AACToolAndroidSSBR;
            profile.nAACtools &= ~OMX_AUDIO_AACToolAndroidDSBR;
 break;
 case 2:
            profile.nAACtools &= ~OMX_AUDIO_AACToolAndroidSSBR;
            profile.nAACtools |= OMX_AUDIO_AACToolAndroidDSBR;
 break;
 case -1:
            profile.nAACtools |= OMX_AUDIO_AACToolAndroidSSBR;
            profile.nAACtools |= OMX_AUDIO_AACToolAndroidDSBR;
 break;
 default:
 return BAD_VALUE;
 }


        err = mOMX->setParameter(
                mNode, OMX_IndexParamAudioAac, &profile, sizeof(profile));

 if (err != OK) {
 return err;
 }

 return err;
 }

    OMX_AUDIO_PARAM_AACPROFILETYPE profile;
 InitOMXParams(&profile);
    profile.nPortIndex = kPortIndexInput;

    err = mOMX->getParameter(
            mNode, OMX_IndexParamAudioAac, &profile, sizeof(profile));

 if (err != OK) {
 return err;
 }

    profile.nChannels = numChannels;
    profile.nSampleRate = sampleRate;

    profile.eAACStreamFormat =
        isADTS
 ? OMX_AUDIO_AACStreamFormatMP4ADTS

             : OMX_AUDIO_AACStreamFormatMP4FF;
 
     OMX_AUDIO_PARAM_ANDROID_AACPRESENTATIONTYPE presentation;
     presentation.nMaxOutputChannels = maxOutputChannelCount;
     presentation.nDrcCut = drc.drcCut;
     presentation.nDrcBoost = drc.drcBoost;
    presentation.nHeavyCompression = drc.heavyCompression;
    presentation.nTargetReferenceLevel = drc.targetRefLevel;
    presentation.nEncodedTargetLevel = drc.encodedTargetLevel;
    presentation.nPCMLimiterEnable = pcmLimiterEnable;

 status_t res = mOMX->setParameter(mNode, OMX_IndexParamAudioAac, &profile, sizeof(profile));
 if (res == OK) {
        mOMX->setParameter(mNode, (OMX_INDEXTYPE)OMX_IndexParamAudioAndroidAacPresentation,
 &presentation, sizeof(presentation));
 } else {
        ALOGW("did not set AudioAndroidAacPresentation due to error %d when setting AudioAac", res);
 }
 return res;
}
