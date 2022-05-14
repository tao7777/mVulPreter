void WT_Interpolate (S_WT_VOICE *pWTVoice, S_WT_INT_FRAME *pWTIntFrame)
{
    EAS_PCM *pOutputBuffer;
    EAS_I32 phaseInc;
    EAS_I32 phaseFrac;
    EAS_I32 acc0;
 const EAS_SAMPLE *pSamples;
 const EAS_SAMPLE *loopEnd;
    EAS_I32 samp1;
    EAS_I32 samp2;
    EAS_I32 numSamples;

 
     /* initialize some local variables */
     numSamples = pWTIntFrame->numSamples;
    if (numSamples <= 0) {
        ALOGE("b/26366256");
        return;
    }
     pOutputBuffer = pWTIntFrame->pAudioBuffer;
 
     loopEnd = (const EAS_SAMPLE*) pWTVoice->loopEnd + 1;
    pSamples = (const EAS_SAMPLE*) pWTVoice->phaseAccum;
 /*lint -e{713} truncation is OK */
    phaseFrac = pWTVoice->phaseFrac;
    phaseInc = pWTIntFrame->frame.phaseIncrement;

 /* fetch adjacent samples */
#if defined(_8_BIT_SAMPLES)
 /*lint -e{701} <avoid multiply for performance>*/
    samp1 = pSamples[0] << 8;
 /*lint -e{701} <avoid multiply for performance>*/
    samp2 = pSamples[1] << 8;
#else
    samp1 = pSamples[0];
    samp2 = pSamples[1];
#endif

 while (numSamples--) {

 /* linear interpolation */
        acc0 = samp2 - samp1;
        acc0 = acc0 * phaseFrac;
 /*lint -e{704} <avoid divide>*/
        acc0 = samp1 + (acc0 >> NUM_PHASE_FRAC_BITS);

 /* save new output sample in buffer */
 /*lint -e{704} <avoid divide>*/
 *pOutputBuffer++ = (EAS_I16)(acc0 >> 2);

 /* increment phase */
        phaseFrac += phaseInc;
 /*lint -e{704} <avoid divide>*/
        acc0 = phaseFrac >> NUM_PHASE_FRAC_BITS;

 /* next sample */
 if (acc0 > 0) {

 /* advance sample pointer */
            pSamples += acc0;
            phaseFrac = (EAS_I32)((EAS_U32)phaseFrac & PHASE_FRAC_MASK);

 /* check for loop end */
            acc0 = (EAS_I32) (pSamples - loopEnd);
 if (acc0 >= 0)
                pSamples = (const EAS_SAMPLE*) pWTVoice->loopStart + acc0;

 /* fetch new samples */
#if defined(_8_BIT_SAMPLES)
 /*lint -e{701} <avoid multiply for performance>*/
            samp1 = pSamples[0] << 8;
 /*lint -e{701} <avoid multiply for performance>*/
            samp2 = pSamples[1] << 8;
#else
            samp1 = pSamples[0];
            samp2 = pSamples[1];
#endif
 }
 }

 /* save pointer and phase */
    pWTVoice->phaseAccum = (EAS_U32) pSamples;
    pWTVoice->phaseFrac = (EAS_U32) phaseFrac;
}
