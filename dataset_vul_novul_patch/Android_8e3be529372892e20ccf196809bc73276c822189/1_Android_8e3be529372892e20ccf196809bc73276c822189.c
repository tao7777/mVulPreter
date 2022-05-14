void lppTransposer (HANDLE_SBR_LPP_TRANS hLppTrans, /*!< Handle of lpp transposer  */
                    QMF_SCALE_FACTOR  *sbrScaleFactor, /*!< Scaling factors */
                    FIXP_DBL **qmfBufferReal, /*!< Pointer to pointer to real part of subband samples (source) */

                    FIXP_DBL *degreeAlias, /*!< Vector for results of aliasing estimation */
                    FIXP_DBL **qmfBufferImag, /*!< Pointer to pointer to imaginary part of subband samples (source) */
 const int useLP,
 const int timeStep, /*!< Time step of envelope */
 const int firstSlotOffs, /*!< Start position in time */
 const int lastSlotOffs, /*!< Number of overlap-slots into next frame */
 const int nInvfBands, /*!< Number of bands for inverse filtering */
                    INVF_MODE *sbr_invf_mode, /*!< Current inverse filtering modes */
                    INVF_MODE *sbr_invf_mode_prev      /*!< Previous inverse filtering modes */
 )
{
  INT    bwIndex[MAX_NUM_PATCHES];
  FIXP_DBL  bwVector[MAX_NUM_PATCHES]; /*!< pole moving factors */

 int    i;
 int    loBand, start, stop;
  TRANSPOSER_SETTINGS *pSettings = hLppTrans->pSettings;
  PATCH_PARAM *patchParam = pSettings->patchParam;
 int    patch;

  FIXP_SGL  alphar[LPC_ORDER], a0r, a1r;
  FIXP_SGL  alphai[LPC_ORDER], a0i=0, a1i=0;
  FIXP_SGL  bw = FL2FXCONST_SGL(0.0f);

 int    autoCorrLength;

  FIXP_DBL k1, k1_below=0, k1_below2=0;

  ACORR_COEFS ac;
 int    startSample;
 int    stopSample;
 int    stopSampleClear;

 int comLowBandScale;
 int ovLowBandShift;
 int lowBandShift;
/*  int ovHighBandShift;*/
 int targetStopBand;


  alphai[0] = FL2FXCONST_SGL(0.0f);
  alphai[1] = FL2FXCONST_SGL(0.0f);


  startSample = firstSlotOffs * timeStep;
  stopSample  = pSettings->nCols + lastSlotOffs * timeStep;


  inverseFilteringLevelEmphasis(hLppTrans, nInvfBands, sbr_invf_mode, sbr_invf_mode_prev, bwVector);

  stopSampleClear = stopSample;

  autoCorrLength = pSettings->nCols + pSettings->overlap;

 /* Set upper subbands to zero:
     This is required in case that the patches do not cover the complete highband
     (because the last patch would be too short).
     Possible optimization: Clearing bands up to usb would be sufficient here. */
  targetStopBand = patchParam[pSettings->noOfPatches-1].targetStartBand
 + patchParam[pSettings->noOfPatches-1].numBandsInPatch;

 int memSize = ((64) - targetStopBand) * sizeof(FIXP_DBL);

 if (!useLP) {
 for (i = startSample; i < stopSampleClear; i++) {
 FDKmemclear(&qmfBufferReal[i][targetStopBand], memSize);
 FDKmemclear(&qmfBufferImag[i][targetStopBand], memSize);
 }
 } else
 for (i = startSample; i < stopSampleClear; i++) {
 FDKmemclear(&qmfBufferReal[i][targetStopBand], memSize);

   }
 
   /* init bwIndex for each patch */
  FDKmemclear(bwIndex, pSettings->noOfPatches*sizeof(INT));
 
   /*
     Calc common low band scale factor
  */
  comLowBandScale = fixMin(sbrScaleFactor->ov_lb_scale,sbrScaleFactor->lb_scale);

  ovLowBandShift =  sbrScaleFactor->ov_lb_scale - comLowBandScale;
  lowBandShift   =  sbrScaleFactor->lb_scale - comLowBandScale;
 /*  ovHighBandShift = firstSlotOffs == 0 ? ovLowBandShift:0;*/

 /* outer loop over bands to do analysis only once for each band */

 if (!useLP) {
    start = pSettings->lbStartPatching;
    stop = pSettings->lbStopPatching;
 } else
 {
    start = fixMax(1, pSettings->lbStartPatching - 2);
    stop = patchParam[0].targetStartBand;
 }


 for ( loBand = start; loBand <  stop; loBand++ ) {

    FIXP_DBL  lowBandReal[(((1024)/(32))+(6))+LPC_ORDER];
    FIXP_DBL *plowBandReal = lowBandReal;
    FIXP_DBL **pqmfBufferReal = qmfBufferReal;
    FIXP_DBL  lowBandImag[(((1024)/(32))+(6))+LPC_ORDER];
    FIXP_DBL *plowBandImag = lowBandImag;
    FIXP_DBL **pqmfBufferImag = qmfBufferImag;
 int resetLPCCoeffs=0;
 int dynamicScale = DFRACT_BITS-1-LPC_SCALE_FACTOR;
 int acDetScale = 0; /* scaling of autocorrelation determinant */

 for(i=0;i<LPC_ORDER;i++){
 *plowBandReal++ = hLppTrans->lpcFilterStatesReal[i][loBand];
 if (!useLP)
 *plowBandImag++ = hLppTrans->lpcFilterStatesImag[i][loBand];
 }

 /*
      Take old slope length qmf slot source values out of (overlap)qmf buffer
    */
 if (!useLP) {
 for(i=0;i<pSettings->nCols+pSettings->overlap;i++){
 *plowBandReal++ = (*pqmfBufferReal++)[loBand];
 *plowBandImag++ = (*pqmfBufferImag++)[loBand];
 }
 } else
 {
 /* pSettings->overlap is always even */
      FDK_ASSERT((pSettings->overlap & 1) == 0);

 for(i=0;i<((pSettings->overlap+pSettings->nCols)>>1);i++) {
 *plowBandReal++ = (*pqmfBufferReal++)[loBand];
 *plowBandReal++ = (*pqmfBufferReal++)[loBand];
 }
 if (pSettings->nCols & 1) {
 *plowBandReal++ = (*pqmfBufferReal++)[loBand];
 }
 }

 /*
      Determine dynamic scaling value.
     */
    dynamicScale = fixMin(dynamicScale, getScalefactor(lowBandReal, LPC_ORDER+pSettings->overlap) + ovLowBandShift);
    dynamicScale = fixMin(dynamicScale, getScalefactor(&lowBandReal[LPC_ORDER+pSettings->overlap], pSettings->nCols) + lowBandShift);
 if (!useLP) {
      dynamicScale = fixMin(dynamicScale, getScalefactor(lowBandImag, LPC_ORDER+pSettings->overlap) + ovLowBandShift);
      dynamicScale = fixMin(dynamicScale, getScalefactor(&lowBandImag[LPC_ORDER+pSettings->overlap], pSettings->nCols) + lowBandShift);
 }
    dynamicScale = fixMax(0, dynamicScale-1); /* one additional bit headroom to prevent -1.0 */

 /*
      Scale temporal QMF buffer.
     */
    scaleValues(&lowBandReal[0], LPC_ORDER+pSettings->overlap, dynamicScale-ovLowBandShift);
    scaleValues(&lowBandReal[LPC_ORDER+pSettings->overlap], pSettings->nCols, dynamicScale-lowBandShift);

 if (!useLP) {
      scaleValues(&lowBandImag[0], LPC_ORDER+pSettings->overlap, dynamicScale-ovLowBandShift);
      scaleValues(&lowBandImag[LPC_ORDER+pSettings->overlap], pSettings->nCols, dynamicScale-lowBandShift);
 }


 if (!useLP) {
        acDetScale += autoCorr2nd_cplx(&ac, lowBandReal+LPC_ORDER, lowBandImag+LPC_ORDER, autoCorrLength);
 }
 else
 {
        acDetScale += autoCorr2nd_real(&ac, lowBandReal+LPC_ORDER, autoCorrLength);
 }

 /* Examine dynamic of determinant in autocorrelation. */
      acDetScale += 2*(comLowBandScale + dynamicScale);
      acDetScale *= 2; /* two times reflection coefficent scaling */
      acDetScale += ac.det_scale; /* ac scaling of determinant */

 /* In case of determinant < 10^-38, resetLPCCoeffs=1 has to be enforced. */
 if (acDetScale>126 ) {
        resetLPCCoeffs = 1;
 }


    alphar[1] = FL2FXCONST_SGL(0.0f);
 if (!useLP)
      alphai[1] = FL2FXCONST_SGL(0.0f);

 if (ac.det != FL2FXCONST_DBL(0.0f)) {
      FIXP_DBL tmp,absTmp,absDet;

      absDet = fixp_abs(ac.det);

 if (!useLP) {
        tmp = ( fMultDiv2(ac.r01r,ac.r12r) >> (LPC_SCALE_FACTOR-1) ) -
 ( (fMultDiv2(ac.r01i,ac.r12i) + fMultDiv2(ac.r02r,ac.r11r)) >> (LPC_SCALE_FACTOR-1) );
 } else
 {
        tmp = ( fMultDiv2(ac.r01r,ac.r12r) >> (LPC_SCALE_FACTOR-1) ) -
 ( fMultDiv2(ac.r02r,ac.r11r) >> (LPC_SCALE_FACTOR-1) );
 }
      absTmp = fixp_abs(tmp);

 /*
        Quick check: is first filter coeff >= 1(4)
       */
 {
        INT scale;
        FIXP_DBL result = fDivNorm(absTmp, absDet, &scale);
        scale = scale+ac.det_scale;

 if ( (scale > 0) && (result >= (FIXP_DBL)MAXVAL_DBL>>scale) ) {
          resetLPCCoeffs = 1;
 }
 else {
          alphar[1] = FX_DBL2FX_SGL(scaleValue(result,scale));
 if((tmp<FL2FX_DBL(0.0f)) ^ (ac.det<FL2FX_DBL(0.0f))) {
            alphar[1] = -alphar[1];
 }
 }
 }

 if (!useLP)
 {
        tmp = ( fMultDiv2(ac.r01i,ac.r12r) >> (LPC_SCALE_FACTOR-1) ) +
 ( (fMultDiv2(ac.r01r,ac.r12i) - (FIXP_DBL)fMultDiv2(ac.r02i,ac.r11r)) >> (LPC_SCALE_FACTOR-1) ) ;

        absTmp = fixp_abs(tmp);

 /*
        Quick check: is second filter coeff >= 1(4)
        */
 {
          INT scale;
          FIXP_DBL result = fDivNorm(absTmp, absDet, &scale);
          scale = scale+ac.det_scale;

 if ( (scale > 0) && (result >= /*FL2FXCONST_DBL(1.f)*/ (FIXP_DBL)MAXVAL_DBL>>scale) ) {
            resetLPCCoeffs = 1;
 }
 else {
            alphai[1] = FX_DBL2FX_SGL(scaleValue(result,scale));
 if((tmp<FL2FX_DBL(0.0f)) ^ (ac.det<FL2FX_DBL(0.0f))) {
              alphai[1] = -alphai[1];
 }
 }
 }
 }
 }

    alphar[0] =  FL2FXCONST_SGL(0.0f);
 if (!useLP)
      alphai[0] = FL2FXCONST_SGL(0.0f);

 if ( ac.r11r != FL2FXCONST_DBL(0.0f) ) {

 /* ac.r11r is always >=0 */
      FIXP_DBL tmp,absTmp;

 if (!useLP) {
        tmp = (ac.r01r>>(LPC_SCALE_FACTOR+1)) +
 (fMultDiv2(alphar[1],ac.r12r) + fMultDiv2(alphai[1],ac.r12i));
 } else
 {
 if(ac.r01r>=FL2FXCONST_DBL(0.0f))
          tmp = (ac.r01r>>(LPC_SCALE_FACTOR+1)) + fMultDiv2(alphar[1],ac.r12r);
 else
          tmp = -((-ac.r01r)>>(LPC_SCALE_FACTOR+1)) + fMultDiv2(alphar[1],ac.r12r);
 }

      absTmp = fixp_abs(tmp);

 /*
        Quick check: is first filter coeff >= 1(4)
      */

 if (absTmp >= (ac.r11r>>1)) {
        resetLPCCoeffs=1;
 }
 else {
        INT scale;
        FIXP_DBL result = fDivNorm(absTmp, fixp_abs(ac.r11r), &scale);
        alphar[0] =  FX_DBL2FX_SGL(scaleValue(result,scale+1));

 if((tmp>FL2FX_DBL(0.0f)) ^ (ac.r11r<FL2FX_DBL(0.0f)))
          alphar[0] = -alphar[0];
 }

 if (!useLP)
 {
        tmp = (ac.r01i>>(LPC_SCALE_FACTOR+1)) +
 (fMultDiv2(alphai[1],ac.r12r) - fMultDiv2(alphar[1],ac.r12i));

        absTmp = fixp_abs(tmp);

 /*
        Quick check: is second filter coeff >= 1(4)
        */
 if (absTmp >= (ac.r11r>>1)) {
          resetLPCCoeffs=1;
 }
 else {
          INT scale;
          FIXP_DBL result = fDivNorm(absTmp, fixp_abs(ac.r11r), &scale);
          alphai[0] = FX_DBL2FX_SGL(scaleValue(result,scale+1));
 if((tmp>FL2FX_DBL(0.0f)) ^ (ac.r11r<FL2FX_DBL(0.0f)))
            alphai[0] = -alphai[0];
 }
 }
 }


 if (!useLP)
 {
 /* Now check the quadratic criteria */
 if( (fMultDiv2(alphar[0],alphar[0]) + fMultDiv2(alphai[0],alphai[0])) >= FL2FXCONST_DBL(0.5f) )
        resetLPCCoeffs=1;
 if( (fMultDiv2(alphar[1],alphar[1]) + fMultDiv2(alphai[1],alphai[1])) >= FL2FXCONST_DBL(0.5f) )
        resetLPCCoeffs=1;
 }

 if(resetLPCCoeffs){
      alphar[0] = FL2FXCONST_SGL(0.0f);
      alphar[1] = FL2FXCONST_SGL(0.0f);
 if (!useLP)
 {
        alphai[0] = FL2FXCONST_SGL(0.0f);
        alphai[1] = FL2FXCONST_SGL(0.0f);
 }
 }

 if (useLP)
 {

 /* Aliasing detection */
 if(ac.r11r==FL2FXCONST_DBL(0.0f)) {
        k1 = FL2FXCONST_DBL(0.0f);
 }
 else {
 if ( fixp_abs(ac.r01r) >= fixp_abs(ac.r11r) ) {
 if ( fMultDiv2(ac.r01r,ac.r11r) < FL2FX_DBL(0.0f)) {
            k1 = (FIXP_DBL)MAXVAL_DBL /*FL2FXCONST_SGL(1.0f)*/;
 }else {
 /* Since this value is squared later, it must not ever become -1.0f. */
            k1 = (FIXP_DBL)(MINVAL_DBL+1) /*FL2FXCONST_SGL(-1.0f)*/;
 }
 }
 else {
          INT scale;
          FIXP_DBL result = fDivNorm(fixp_abs(ac.r01r), fixp_abs(ac.r11r), &scale);
          k1 = scaleValue(result,scale);

 if(!((ac.r01r<FL2FX_DBL(0.0f)) ^ (ac.r11r<FL2FX_DBL(0.0f)))) {
            k1 = -k1;
 }
 }
 }
 if(loBand > 1){
 /* Check if the gain should be locked */
        FIXP_DBL deg = /*FL2FXCONST_DBL(1.0f)*/ (FIXP_DBL)MAXVAL_DBL - fPow2(k1_below);
        degreeAlias[loBand] = FL2FXCONST_DBL(0.0f);
 if (((loBand & 1) == 0) && (k1 < FL2FXCONST_DBL(0.0f))){
 if (k1_below < FL2FXCONST_DBL(0.0f)) { /* 2-Ch Aliasing Detection */
            degreeAlias[loBand] = (FIXP_DBL)MAXVAL_DBL /*FL2FXCONST_DBL(1.0f)*/;
 if ( k1_below2 > FL2FXCONST_DBL(0.0f) ) { /* 3-Ch Aliasing Detection */
              degreeAlias[loBand-1] = deg;
 }
 }
 else if ( k1_below2 > FL2FXCONST_DBL(0.0f) ) { /* 3-Ch Aliasing Detection */
            degreeAlias[loBand] = deg;
 }
 }
 if (((loBand & 1) == 1) && (k1 > FL2FXCONST_DBL(0.0f))){
 if (k1_below > FL2FXCONST_DBL(0.0f)) { /* 2-CH Aliasing Detection */
            degreeAlias[loBand] = (FIXP_DBL)MAXVAL_DBL /*FL2FXCONST_DBL(1.0f)*/;
 if ( k1_below2 < FL2FXCONST_DBL(0.0f) ) { /* 3-CH Aliasing Detection */
              degreeAlias[loBand-1] = deg;
 }
 }
 else if ( k1_below2 < FL2FXCONST_DBL(0.0f) ) { /* 3-CH Aliasing Detection */
            degreeAlias[loBand] = deg;
 }
 }
 }
 /* remember k1 values of the 2 QMF channels below the current channel */
      k1_below2 = k1_below;
      k1_below = k1;
 }

    patch = 0;

 while ( patch < pSettings->noOfPatches ) { /* inner loop over every patch */

 int hiBand = loBand + patchParam[patch].targetBandOffs;

 if ( loBand < patchParam[patch].sourceStartBand
 || loBand >= patchParam[patch].sourceStopBand
 ) {
 /* Lowband not in current patch - proceed */
        patch++;
 continue;
 }


       FDK_ASSERT( hiBand < (64) );
 
       /* bwIndex[patch] is already initialized with value from previous band inside this patch */
      while (hiBand >= pSettings->bwBorders[bwIndex[patch]])
         bwIndex[patch]++;
 
       /*
         Filter Step 2: add the left slope with the current filter to the buffer
                       pure source values are already in there
      */
      bw = FX_DBL2FX_SGL(bwVector[bwIndex[patch]]);

      a0r = FX_DBL2FX_SGL(fMult(bw,alphar[0])); /* Apply current bandwidth expansion factor */


 if (!useLP)
        a0i = FX_DBL2FX_SGL(fMult(bw,alphai[0]));
      bw =  FX_DBL2FX_SGL(fPow2(bw));
      a1r = FX_DBL2FX_SGL(fMult(bw,alphar[1]));
 if (!useLP)
        a1i = FX_DBL2FX_SGL(fMult(bw,alphai[1]));



 /*
        Filter Step 3: insert the middle part which won't be windowed
      */

 if ( bw <= FL2FXCONST_SGL(0.0f) ) {
 if (!useLP) {
 int descale = fixMin(DFRACT_BITS-1, (LPC_SCALE_FACTOR+dynamicScale));
 for(i = startSample; i < stopSample; i++ ) {
            qmfBufferReal[i][hiBand] = lowBandReal[LPC_ORDER+i]>>descale;
            qmfBufferImag[i][hiBand] = lowBandImag[LPC_ORDER+i]>>descale;
 }
 } else
 {
 int descale = fixMin(DFRACT_BITS-1, (LPC_SCALE_FACTOR+dynamicScale));
 for(i = startSample; i < stopSample; i++ ) {
            qmfBufferReal[i][hiBand] = lowBandReal[LPC_ORDER+i]>>descale;
 }
 }
 }
 else { /* bw <= 0 */

 if (!useLP) {
 int descale = fixMin(DFRACT_BITS-1, (LPC_SCALE_FACTOR+dynamicScale));
#ifdef FUNCTION_LPPTRANSPOSER_func1
          lppTransposer_func1(lowBandReal+LPC_ORDER+startSample,lowBandImag+LPC_ORDER+startSample,
                              qmfBufferReal+startSample,qmfBufferImag+startSample,
                              stopSample-startSample, (int) hiBand,
                              dynamicScale,descale,
                              a0r, a0i, a1r, a1i);
#else
 for(i = startSample; i < stopSample; i++ ) {
            FIXP_DBL accu1, accu2;

            accu1 = (fMultDiv2(a0r,lowBandReal[LPC_ORDER+i-1]) - fMultDiv2(a0i,lowBandImag[LPC_ORDER+i-1]) +
                     fMultDiv2(a1r,lowBandReal[LPC_ORDER+i-2]) - fMultDiv2(a1i,lowBandImag[LPC_ORDER+i-2]))>>dynamicScale;
            accu2 = (fMultDiv2(a0i,lowBandReal[LPC_ORDER+i-1]) + fMultDiv2(a0r,lowBandImag[LPC_ORDER+i-1]) +
                     fMultDiv2(a1i,lowBandReal[LPC_ORDER+i-2]) + fMultDiv2(a1r,lowBandImag[LPC_ORDER+i-2]))>>dynamicScale;

            qmfBufferReal[i][hiBand] = (lowBandReal[LPC_ORDER+i]>>descale) + (accu1<<1);
            qmfBufferImag[i][hiBand] = (lowBandImag[LPC_ORDER+i]>>descale) + (accu2<<1);
 }
#endif
 } else
 {
 int descale = fixMin(DFRACT_BITS-1, (LPC_SCALE_FACTOR+dynamicScale));

          FDK_ASSERT(dynamicScale >= 0);
 for(i = startSample; i < stopSample; i++ ) {
            FIXP_DBL accu1;

            accu1 = (fMultDiv2(a0r,lowBandReal[LPC_ORDER+i-1]) + fMultDiv2(a1r,lowBandReal[LPC_ORDER+i-2]))>>dynamicScale;

            qmfBufferReal[i][hiBand] = (lowBandReal[LPC_ORDER+i]>>descale) + (accu1<<1);
 }
 }
 } /* bw <= 0 */

      patch++;

 } /* inner loop over patches */

 /*
     * store the unmodified filter coefficients if there is
     * an overlapping envelope
     *****************************************************************/


 } /* outer loop over bands (loBand) */

 if (useLP)
 {
 for ( loBand = pSettings->lbStartPatching; loBand <  pSettings->lbStopPatching; loBand++ ) {
      patch = 0;
 while ( patch < pSettings->noOfPatches ) {

        UCHAR hiBand = loBand + patchParam[patch].targetBandOffs;

 if ( loBand < patchParam[patch].sourceStartBand
 || loBand >= patchParam[patch].sourceStopBand
 || hiBand >= (64) /* Highband out of range (biterror) */
 ) {
 /* Lowband not in current patch or highband out of range (might be caused by biterrors)- proceed */
          patch++;
 continue;
 }

 if(hiBand != patchParam[patch].targetStartBand)
          degreeAlias[hiBand] = degreeAlias[loBand];

        patch++;
 }
 }/* end  for loop */
 }

 for (i = 0; i < nInvfBands; i++ ) {
   hLppTrans->bwVectorOld[i] = bwVector[i];
 }

 /*
    set high band scale factor
  */
  sbrScaleFactor->hb_scale = comLowBandScale-(LPC_SCALE_FACTOR);

}
