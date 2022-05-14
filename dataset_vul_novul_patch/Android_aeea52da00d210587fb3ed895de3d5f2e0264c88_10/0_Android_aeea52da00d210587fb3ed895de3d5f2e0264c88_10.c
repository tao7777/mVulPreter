 int PreProcessingFx_ProcessReverse(effect_handle_t     self,
                                    audio_buffer_t    *inBuffer,
                                   audio_buffer_t    *outBuffer __unused)
 {
     preproc_effect_t * effect = (preproc_effect_t *)self;
     int    status = 0;

 if (effect == NULL){
        ALOGW("PreProcessingFx_ProcessReverse() ERROR effect == NULL");
 return -EINVAL;
 }
 preproc_session_t * session = (preproc_session_t *)effect->session;

 if (inBuffer == NULL  || inBuffer->raw == NULL){
        ALOGW("PreProcessingFx_ProcessReverse() ERROR bad pointer");
 return -EINVAL;
 }

    session->revProcessedMsk |= (1<<effect->procId);



 if ((session->revProcessedMsk & session->revEnabledMsk) == session->revEnabledMsk) {
        effect->session->revProcessedMsk = 0;
 if (session->revResampler != NULL) {
 size_t fr = session->frameCount - session->framesRev;
 if (inBuffer->frameCount < fr) {
                fr = inBuffer->frameCount;
 }
 if (session->revBufSize < session->framesRev + fr) {
                session->revBufSize = session->framesRev + fr;
                session->revBuf = (int16_t *)realloc(session->revBuf,
                                  session->revBufSize * session->inChannelCount * sizeof(int16_t));
 }
            memcpy(session->revBuf + session->framesRev * session->inChannelCount,
                   inBuffer->s16,
                   fr * session->inChannelCount * sizeof(int16_t));

            session->framesRev += fr;
            inBuffer->frameCount = fr;
 if (session->framesRev < session->frameCount) {
 return 0;
 }
 spx_uint32_t frIn = session->framesRev;
 spx_uint32_t frOut = session->apmFrameCount;
 if (session->inChannelCount == 1) {
                speex_resampler_process_int(session->revResampler,
 0,
                                            session->revBuf,
 &frIn,
                                            session->revFrame->_payloadData,
 &frOut);
 } else {
                speex_resampler_process_interleaved_int(session->revResampler,
                                                        session->revBuf,
 &frIn,
                                                        session->revFrame->_payloadData,
 &frOut);
 }
            memcpy(session->revBuf,
                   session->revBuf + frIn * session->inChannelCount,
 (session->framesRev - frIn) * session->inChannelCount * sizeof(int16_t));
            session->framesRev -= frIn;
 } else {
 size_t fr = session->frameCount - session->framesRev;
 if (inBuffer->frameCount < fr) {
                fr = inBuffer->frameCount;
 }
            memcpy(session->revFrame->_payloadData + session->framesRev * session->inChannelCount,
                   inBuffer->s16,
                   fr * session->inChannelCount * sizeof(int16_t));
            session->framesRev += fr;
            inBuffer->frameCount = fr;
 if (session->framesRev < session->frameCount) {
 return 0;
 }
            session->framesRev = 0;
 }
        session->revFrame->_payloadDataLengthInSamples =
                session->apmFrameCount * session->inChannelCount;
        effect->session->apm->AnalyzeReverseStream(session->revFrame);
 return 0;
 } else {
 return -ENODATA;
 }
}
