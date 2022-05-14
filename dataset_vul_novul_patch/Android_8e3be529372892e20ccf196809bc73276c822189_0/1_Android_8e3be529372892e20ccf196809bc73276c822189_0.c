resetLppTransposer (HANDLE_SBR_LPP_TRANS hLppTrans, /*!< Handle of lpp transposer  */
                    UCHAR  highBandStartSb, /*!< High band area: start subband */
                    UCHAR *v_k_master, /*!< Master table */
                    UCHAR  numMaster, /*!< Valid entries in master table */
                    UCHAR *noiseBandTable, /*!< Mapping of SBR noise bands to QMF bands */
                    UCHAR  noNoiseBands, /*!< Number of noise bands */
                    UCHAR  usb, /*!< High band area: stop subband */
                    UINT   fs                        /*!< SBR output sampling frequency */
 )
{
  TRANSPOSER_SETTINGS *pSettings = hLppTrans->pSettings;
  PATCH_PARAM  *patchParam = pSettings->patchParam;

 int i, patch;
 int targetStopBand;
 int sourceStartBand;
 int patchDistance;
 int numBandsInPatch;

 int lsb = v_k_master[0]; /* Start subband expressed in "non-critical" sampling terms*/
 int xoverOffset = highBandStartSb - lsb; /* Calculate distance in QMF bands between k0 and kx */
 int startFreqHz;

 int desiredBorder;

  usb = fixMin(usb, v_k_master[numMaster]); /* Avoid endless loops (compare with float code). */

 /*
   * Plausibility check
   */

 if ( lsb - SHIFT_START_SB < 4 ) {
 return SBRDEC_UNSUPPORTED_CONFIG;
 }


 /*
   * Initialize the patching parameter
   */
 /* ISO/IEC 14496-3 (Figure 4.48): goalSb = round( 2.048e6 / fs ) */
  desiredBorder    = (((2048000*2) / fs) + 1) >> 1;

  desiredBorder = findClosestEntry(desiredBorder, v_k_master, numMaster, 1); /* Adapt region to master-table */

 /* First patch */
  sourceStartBand = SHIFT_START_SB + xoverOffset;
  targetStopBand = lsb + xoverOffset; /* upperBand */

 /* Even (odd) numbered channel must be patched to even (odd) numbered channel */
  patch = 0;
 while(targetStopBand < usb) {

 /* Too many patches?
       Allow MAX_NUM_PATCHES+1 patches here.
       we need to check later again, since patch might be the highest patch
       AND contain less than 3 bands => actual number of patches will be reduced by 1.
    */
 if (patch > MAX_NUM_PATCHES) {
 return SBRDEC_UNSUPPORTED_CONFIG;
 }

    patchParam[patch].guardStartBand = targetStopBand;
    patchParam[patch].targetStartBand = targetStopBand;

    numBandsInPatch = desiredBorder - targetStopBand; /* Get the desired range of the patch */

 if ( numBandsInPatch >= lsb - sourceStartBand ) {
 /* Desired number bands are not available -> patch whole source range */
      patchDistance   = targetStopBand - sourceStartBand; /* Get the targetOffset */
      patchDistance   = patchDistance & ~1; /* Rounding off odd numbers and make all even */
      numBandsInPatch = lsb - (targetStopBand - patchDistance); /* Update number of bands to be patched */
      numBandsInPatch = findClosestEntry(targetStopBand + numBandsInPatch, v_k_master, numMaster, 0) -
                        targetStopBand; /* Adapt region to master-table */
 }

 /* Desired number bands are available -> get the minimal even patching distance */
    patchDistance   = numBandsInPatch + targetStopBand - lsb; /* Get minimal distance */
    patchDistance   = (patchDistance + 1) & ~1; /* Rounding up odd numbers and make all even */

 if (numBandsInPatch > 0) {
      patchParam[patch].sourceStartBand = targetStopBand - patchDistance;
      patchParam[patch].targetBandOffs  = patchDistance;
      patchParam[patch].numBandsInPatch = numBandsInPatch;
      patchParam[patch].sourceStopBand  = patchParam[patch].sourceStartBand + numBandsInPatch;

      targetStopBand += patchParam[patch].numBandsInPatch;
      patch++;
 }

 /* All patches but first */
    sourceStartBand = SHIFT_START_SB;

 /* Check if we are close to desiredBorder */
 if( desiredBorder - targetStopBand < 3) /* MPEG doc */
 {
      desiredBorder = usb;
 }

 }

  patch--;

 /* If highest patch contains less than three subband: skip it */
 if ( (patch>0) && (patchParam[patch].numBandsInPatch < 3) ) {
    patch--;
    targetStopBand = patchParam[patch].targetStartBand + patchParam[patch].numBandsInPatch;
 }

 /* now check if we don't have one too many */
 if (patch >= MAX_NUM_PATCHES) {
 return SBRDEC_UNSUPPORTED_CONFIG;
 }

  pSettings->noOfPatches = patch + 1;

 /* Check lowest and highest source subband */
  pSettings->lbStartPatching = targetStopBand;
  pSettings->lbStopPatching  = 0;
 for ( patch = 0; patch < pSettings->noOfPatches; patch++ ) {
    pSettings->lbStartPatching = fixMin( pSettings->lbStartPatching, patchParam[patch].sourceStartBand );
    pSettings->lbStopPatching  = fixMax( pSettings->lbStopPatching, patchParam[patch].sourceStopBand );
 }


   for(i = 0 ; i < noNoiseBands; i++){
     pSettings->bwBorders[i] = noiseBandTable[i+1];
   }
 
   /*
    * Choose whitening factors
   */

  startFreqHz = ( (lsb + xoverOffset)*fs ) >> 7; /* Shift does a division by 2*(64) */

 for( i = 1; i < NUM_WHFACTOR_TABLE_ENTRIES; i++ )
 {
 if( startFreqHz < FDK_sbrDecoder_sbr_whFactorsIndex[i])
 break;
 }
  i--;

  pSettings->whFactors.off = FDK_sbrDecoder_sbr_whFactorsTable[i][0];
  pSettings->whFactors.transitionLevel = FDK_sbrDecoder_sbr_whFactorsTable[i][1];
  pSettings->whFactors.lowLevel = FDK_sbrDecoder_sbr_whFactorsTable[i][2];
  pSettings->whFactors.midLevel = FDK_sbrDecoder_sbr_whFactorsTable[i][3];
  pSettings->whFactors.highLevel = FDK_sbrDecoder_sbr_whFactorsTable[i][4];

 return SBRDEC_OK;
}
