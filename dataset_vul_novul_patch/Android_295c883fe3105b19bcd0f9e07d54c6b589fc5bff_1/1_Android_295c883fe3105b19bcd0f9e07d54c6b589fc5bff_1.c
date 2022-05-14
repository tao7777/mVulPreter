OMX_ERRORTYPE SoftAAC2::internalSetParameter(
        OMX_INDEXTYPE index, const OMX_PTR params) {
 switch ((int)index) {
 case OMX_IndexParamStandardComponentRole:
 {

             const OMX_PARAM_COMPONENTROLETYPE *roleParams =
                 (const OMX_PARAM_COMPONENTROLETYPE *)params;
 
             if (strncmp((const char *)roleParams->cRole,
                         "audio_decoder.aac",
                         OMX_MAX_STRINGNAME_SIZE - 1)) {
 return OMX_ErrorUndefined;
 }

 return OMX_ErrorNone;
 }

 case OMX_IndexParamAudioAac:
 {

             const OMX_AUDIO_PARAM_AACPROFILETYPE *aacParams =
                 (const OMX_AUDIO_PARAM_AACPROFILETYPE *)params;
 
             if (aacParams->nPortIndex != 0) {
                 return OMX_ErrorUndefined;
             }

 if (aacParams->eAACStreamFormat == OMX_AUDIO_AACStreamFormatMP4FF) {
                mIsADTS = false;
 } else if (aacParams->eAACStreamFormat
 == OMX_AUDIO_AACStreamFormatMP4ADTS) {
                mIsADTS = true;
 } else {
 return OMX_ErrorUndefined;
 }

 return OMX_ErrorNone;
 }

 case OMX_IndexParamAudioAndroidAacPresentation:

         {
             const OMX_AUDIO_PARAM_ANDROID_AACPRESENTATIONTYPE *aacPresParams =
                     (const OMX_AUDIO_PARAM_ANDROID_AACPRESENTATIONTYPE *)params;
 if (aacPresParams->nMaxOutputChannels >= 0) {
 int max;
 if (aacPresParams->nMaxOutputChannels >= 8) { max = 8; }
 else if (aacPresParams->nMaxOutputChannels >= 6) { max = 6; }
 else if (aacPresParams->nMaxOutputChannels >= 2) { max = 2; }
 else {
                    max = aacPresParams->nMaxOutputChannels;
 }
                ALOGV("set nMaxOutputChannels=%d", max);
                aacDecoder_SetParam(mAACDecoder, AAC_PCM_MAX_OUTPUT_CHANNELS, max);
 }
 bool updateDrcWrapper = false;
 if (aacPresParams->nDrcBoost >= 0) {
                ALOGV("set nDrcBoost=%d", aacPresParams->nDrcBoost);
                mDrcWrap.setParam(DRC_PRES_MODE_WRAP_DESIRED_BOOST_FACTOR,
                        aacPresParams->nDrcBoost);
                updateDrcWrapper = true;
 }
 if (aacPresParams->nDrcCut >= 0) {
                ALOGV("set nDrcCut=%d", aacPresParams->nDrcCut);
                mDrcWrap.setParam(DRC_PRES_MODE_WRAP_DESIRED_ATT_FACTOR, aacPresParams->nDrcCut);
                updateDrcWrapper = true;
 }
 if (aacPresParams->nHeavyCompression >= 0) {
                ALOGV("set nHeavyCompression=%d", aacPresParams->nHeavyCompression);
                mDrcWrap.setParam(DRC_PRES_MODE_WRAP_DESIRED_HEAVY,
                        aacPresParams->nHeavyCompression);
                updateDrcWrapper = true;
 }
 if (aacPresParams->nTargetReferenceLevel >= 0) {
                ALOGV("set nTargetReferenceLevel=%d", aacPresParams->nTargetReferenceLevel);
                mDrcWrap.setParam(DRC_PRES_MODE_WRAP_DESIRED_TARGET,
                        aacPresParams->nTargetReferenceLevel);
                updateDrcWrapper = true;
 }
 if (aacPresParams->nEncodedTargetLevel >= 0) {
                ALOGV("set nEncodedTargetLevel=%d", aacPresParams->nEncodedTargetLevel);
                mDrcWrap.setParam(DRC_PRES_MODE_WRAP_ENCODER_TARGET,
                        aacPresParams->nEncodedTargetLevel);
                updateDrcWrapper = true;
 }
 if (aacPresParams->nPCMLimiterEnable >= 0) {
                aacDecoder_SetParam(mAACDecoder, AAC_PCM_LIMITER_ENABLE,
 (aacPresParams->nPCMLimiterEnable != 0));
 }
 if (updateDrcWrapper) {
                mDrcWrap.update();
 }

 return OMX_ErrorNone;
 }

 case OMX_IndexParamAudioPcm:
 {

             const OMX_AUDIO_PARAM_PCMMODETYPE *pcmParams =
                 (OMX_AUDIO_PARAM_PCMMODETYPE *)params;
 
             if (pcmParams->nPortIndex != 1) {
                 return OMX_ErrorUndefined;
             }

 return OMX_ErrorNone;
 }

 default:
 return SimpleSoftOMXComponent::internalSetParameter(index, params);
 }
}
