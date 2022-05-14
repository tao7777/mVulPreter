void WT_VoiceGain (S_WT_VOICE *pWTVoice, S_WT_INT_FRAME *pWTIntFrame)
{
    EAS_I32 *pMixBuffer;
    EAS_PCM *pInputBuffer;
    EAS_I32 gain;
    EAS_I32 gainIncrement;
    EAS_I32 tmp0;
    EAS_I32 tmp1;
    EAS_I32 tmp2;
    EAS_I32 numSamples;

#if (NUM_OUTPUT_CHANNELS == 2)
    EAS_I32 gainLeft, gainRight;
#endif

 
     /* initialize some local variables */
     numSamples = pWTIntFrame->numSamples;
     pMixBuffer = pWTIntFrame->pMixBuffer;
     pInputBuffer = pWTIntFrame->pAudioBuffer;
 
 /*lint -e{703} <avoid multiply for performance>*/
    gainIncrement = (pWTIntFrame->frame.gainTarget - pWTIntFrame->prevGain) << (16 - SYNTH_UPDATE_PERIOD_IN_BITS);
 if (gainIncrement < 0)
        gainIncrement++;
 /*lint -e{703} <avoid multiply for performance>*/
    gain = pWTIntFrame->prevGain << 16;

#if (NUM_OUTPUT_CHANNELS == 2)
    gainLeft = pWTVoice->gainLeft;
    gainRight = pWTVoice->gainRight;
#endif

 while (numSamples--) {

 /* incremental gain step to prevent zipper noise */
        tmp0 = *pInputBuffer++;
        gain += gainIncrement;
 /*lint -e{704} <avoid divide>*/
        tmp2 = gain >> 16;

 /* scale sample by gain */
        tmp2 *= tmp0;


 /* stereo output */
#if (NUM_OUTPUT_CHANNELS == 2)
 /*lint -e{704} <avoid divide>*/
        tmp2 = tmp2 >> 14;

 /* get the current sample in the final mix buffer */
        tmp1 = *pMixBuffer;

 /* left channel */
        tmp0 = tmp2 * gainLeft;
 /*lint -e{704} <avoid divide>*/
        tmp0 = tmp0 >> NUM_MIXER_GUARD_BITS;
        tmp1 += tmp0;
 *pMixBuffer++ = tmp1;

 /* get the current sample in the final mix buffer */
        tmp1 = *pMixBuffer;

 /* right channel */
        tmp0 = tmp2 * gainRight;
 /*lint -e{704} <avoid divide>*/
        tmp0 = tmp0 >> NUM_MIXER_GUARD_BITS;
        tmp1 += tmp0;
 *pMixBuffer++ = tmp1;

 /* mono output */
#else

 /* get the current sample in the final mix buffer */
        tmp1 = *pMixBuffer;
 /*lint -e{704} <avoid divide>*/
        tmp2 = tmp2 >> (NUM_MIXER_GUARD_BITS - 1);
        tmp1 += tmp2;
 *pMixBuffer++ = tmp1;
#endif

 }
}
