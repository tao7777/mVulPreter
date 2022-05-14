OMX_ERRORTYPE SoftAMRWBEncoder::internalSetParameter(
        OMX_INDEXTYPE index, const OMX_PTR params) {
 switch (index) {
 case OMX_IndexParamStandardComponentRole:
 {

             const OMX_PARAM_COMPONENTROLETYPE *roleParams =
                 (const OMX_PARAM_COMPONENTROLETYPE *)params;
 
            if (!isValidOMXParam(roleParams)) {
                return OMX_ErrorBadParameter;
            }

             if (strncmp((const char *)roleParams->cRole,
                         "audio_encoder.amrwb",
                         OMX_MAX_STRINGNAME_SIZE - 1)) {
 return OMX_ErrorUndefined;
 }

 return OMX_ErrorNone;
 }

 case OMX_IndexParamAudioPortFormat:
 {

             const OMX_AUDIO_PARAM_PORTFORMATTYPE *formatParams =
                 (const OMX_AUDIO_PARAM_PORTFORMATTYPE *)params;
 
            if (!isValidOMXParam(formatParams)) {
                return OMX_ErrorBadParameter;
            }

             if (formatParams->nPortIndex > 1) {
                 return OMX_ErrorUndefined;
             }

 if (formatParams->nIndex > 0) {
 return OMX_ErrorNoMore;
 }

 if ((formatParams->nPortIndex == 0
 && formatParams->eEncoding != OMX_AUDIO_CodingPCM)
 || (formatParams->nPortIndex == 1
 && formatParams->eEncoding != OMX_AUDIO_CodingAMR)) {
 return OMX_ErrorUndefined;
 }

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

 if (amrParams->nChannels != 1
 || amrParams->eAMRDTXMode != OMX_AUDIO_AMRDTXModeOff
 || amrParams->eAMRFrameFormat
 != OMX_AUDIO_AMRFrameFormatFSF
 || amrParams->eAMRBandMode < OMX_AUDIO_AMRBandModeWB0
 || amrParams->eAMRBandMode > OMX_AUDIO_AMRBandModeWB8) {
 return OMX_ErrorUndefined;
 }

            mBitRate = amrParams->nBitRate;

            mMode = (VOAMRWBMODE)(
                    amrParams->eAMRBandMode - OMX_AUDIO_AMRBandModeWB0);

            amrParams->eAMRDTXMode = OMX_AUDIO_AMRDTXModeOff;
            amrParams->eAMRFrameFormat = OMX_AUDIO_AMRFrameFormatFSF;

 if (VO_ERR_NONE !=
                    mApiHandle->SetParam(
                        mEncoderHandle, VO_PID_AMRWB_MODE, &mMode)) {
                ALOGE("Failed to set AMRWB encoder mode to %d", mMode);
 return OMX_ErrorUndefined;
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

             if (pcmParams->nPortIndex != 0) {
                 return OMX_ErrorUndefined;
             }

 if (pcmParams->nChannels != 1
 || pcmParams->nSamplingRate != (OMX_U32)kSampleRate) {
 return OMX_ErrorUndefined;
 }

 return OMX_ErrorNone;
 }


 default:
 return SimpleSoftOMXComponent::internalSetParameter(index, params);
 }
}
