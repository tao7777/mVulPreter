 void WT_NoiseGenerator (S_WT_VOICE *pWTVoice, S_WT_INT_FRAME *pWTIntFrame)
 {
    EAS_PCM *pOutputBuffer;
    EAS_I32 phaseInc;
    EAS_I32 tmp0;
    EAS_I32 tmp1;
    EAS_I32 nInterpolatedSample;
    EAS_I32 numSamples;

 /* initialize some local variables */

     numSamples = pWTIntFrame->numSamples;
     if (numSamples <= 0) {
         ALOGE("b/26366256");
        android_errorWriteLog(0x534e4554, "26366256");
         return;
     }
     pOutputBuffer = pWTIntFrame->pAudioBuffer;
    phaseInc = pWTIntFrame->frame.phaseIncrement;

 /* get last two samples generated */
 /*lint -e{704} <avoid divide for performance>*/
    tmp0 = (EAS_I32) (pWTVoice->phaseAccum) >> 18;
 /*lint -e{704} <avoid divide for performance>*/
    tmp1 = (EAS_I32) (pWTVoice->loopEnd) >> 18;

 /* generate a buffer of noise */
 while (numSamples--) {
        nInterpolatedSample = MULT_AUDIO_COEF( tmp0, (PHASE_ONE - pWTVoice->phaseFrac));
        nInterpolatedSample += MULT_AUDIO_COEF( tmp1, pWTVoice->phaseFrac);
 *pOutputBuffer++ = (EAS_PCM) nInterpolatedSample;

 /* update PRNG */
        pWTVoice->phaseFrac += (EAS_U32) phaseInc;
 if (GET_PHASE_INT_PART(pWTVoice->phaseFrac)) {
            tmp0 = tmp1;
            pWTVoice->phaseAccum = pWTVoice->loopEnd;
            pWTVoice->loopEnd = (5 * pWTVoice->loopEnd + 1);
            tmp1 = (EAS_I32) (pWTVoice->loopEnd) >> 18;
            pWTVoice->phaseFrac = GET_PHASE_FRAC_PART(pWTVoice->phaseFrac);
 }

 }
}
