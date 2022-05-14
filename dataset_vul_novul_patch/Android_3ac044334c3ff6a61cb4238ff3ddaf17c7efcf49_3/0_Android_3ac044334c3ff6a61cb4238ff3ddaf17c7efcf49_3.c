void WT_VoiceFilter (S_FILTER_CONTROL *pFilter, S_WT_INT_FRAME *pWTIntFrame)
{
    EAS_PCM *pAudioBuffer;
    EAS_I32 k;
    EAS_I32 b1;
    EAS_I32 b2;
    EAS_I32 z1;
    EAS_I32 z2;
    EAS_I32 acc0;
    EAS_I32 acc1;
    EAS_I32 numSamples;

 
     /* initialize some local variables */
     numSamples = pWTIntFrame->numSamples;
    if (numSamples <= 0) {
        ALOGE("b/26366256");
        return;
    }
     pAudioBuffer = pWTIntFrame->pAudioBuffer;
 
     z1 = pFilter->z1;
    z2 = pFilter->z2;
    b1 = -pWTIntFrame->frame.b1;

 /*lint -e{702} <avoid divide> */
    b2 = -pWTIntFrame->frame.b2 >> 1;

 /*lint -e{702} <avoid divide> */
    k = pWTIntFrame->frame.k >> 1;

 while (numSamples--)
 {

 /* do filter calculations */
        acc0 = *pAudioBuffer;
        acc1 = z1 * b1;
        acc1 += z2 * b2;
        acc0 = acc1 + k * acc0;
        z2 = z1;

 /*lint -e{702} <avoid divide> */
        z1 = acc0 >> 14;
 *pAudioBuffer++ = (EAS_I16) z1;
 }

 /* save delay values     */
    pFilter->z1 = (EAS_I16) z1;
    pFilter->z2 = (EAS_I16) z2;
}
