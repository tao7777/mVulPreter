void WT_InterpolateMono (S_WT_VOICE *pWTVoice, S_WT_INT_FRAME *pWTIntFrame)
{
    EAS_I32 *pMixBuffer;
 const EAS_I8 *pLoopEnd;
 const EAS_I8 *pCurrentPhaseInt;
    EAS_I32 numSamples;
    EAS_I32 gain;
    EAS_I32 gainIncrement;
    EAS_I32 currentPhaseFrac;
    EAS_I32 phaseInc;
    EAS_I32 tmp0;
    EAS_I32 tmp1;
    EAS_I32 tmp2;
    EAS_I8 *pLoopStart;


     numSamples = pWTIntFrame->numSamples;
     if (numSamples <= 0) {
         ALOGE("b/26366256");
        android_errorWriteLog(0x534e4554, "26366256");
         return;
     }
     pMixBuffer = pWTIntFrame->pMixBuffer;

 /* calculate gain increment */
    gainIncrement = (pWTIntFrame->gainTarget - pWTIntFrame->prevGain) << (16 - SYNTH_UPDATE_PERIOD_IN_BITS);
 if (gainIncrement < 0)
        gainIncrement++;
    gain = pWTIntFrame->prevGain << 16;

    pCurrentPhaseInt = pWTVoice->pPhaseAccum;
    currentPhaseFrac = pWTVoice->phaseFrac;
    phaseInc = pWTIntFrame->phaseIncrement;

    pLoopStart = pWTVoice->pLoopStart;
    pLoopEnd = pWTVoice->pLoopEnd + 1;

InterpolationLoop:
    tmp0 = (EAS_I32)(pCurrentPhaseInt - pLoopEnd);
 if (tmp0 >= 0)
        pCurrentPhaseInt = pLoopStart + tmp0;

    tmp0 = *pCurrentPhaseInt;
    tmp1 = *(pCurrentPhaseInt + 1);

    tmp2 = phaseInc + currentPhaseFrac;

    tmp1 = tmp1 - tmp0;
    tmp1 = tmp1 * currentPhaseFrac;

    tmp1 = tmp0 + (tmp1 >> NUM_EG1_FRAC_BITS);

    pCurrentPhaseInt += (tmp2 >> NUM_PHASE_FRAC_BITS);
    currentPhaseFrac = tmp2 & PHASE_FRAC_MASK;

    gain += gainIncrement;
    tmp2 = (gain >> SYNTH_UPDATE_PERIOD_IN_BITS);

    tmp0 = *pMixBuffer;
    tmp2 = tmp1 * tmp2;
    tmp2 = (tmp2 >> 9);
    tmp0 = tmp2 + tmp0;
 *pMixBuffer++ = tmp0;

    numSamples--;
 if (numSamples > 0)
 goto InterpolationLoop;

    pWTVoice->pPhaseAccum = pCurrentPhaseInt;
    pWTVoice->phaseFrac = currentPhaseFrac;
 /*lint -e{702} <avoid divide>*/
    pWTVoice->gain = (EAS_I16)(gain >> SYNTH_UPDATE_PERIOD_IN_BITS);
}
