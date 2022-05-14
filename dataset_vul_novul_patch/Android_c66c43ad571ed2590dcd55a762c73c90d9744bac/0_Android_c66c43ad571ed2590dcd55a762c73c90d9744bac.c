int Equalizer_getParameter(EffectContext *pContext,
 void *pParam,
 uint32_t *pValueSize,
 void *pValue){
 int status = 0;
 int bMute = 0;
 int32_t *pParamTemp = (int32_t *)pParam;
 int32_t param = *pParamTemp++;
 int32_t param2;
 char *name;


 switch (param) {
 case EQ_PARAM_NUM_BANDS:
 case EQ_PARAM_CUR_PRESET:
 case EQ_PARAM_GET_NUM_OF_PRESETS:
 case EQ_PARAM_BAND_LEVEL:
 case EQ_PARAM_GET_BAND:
 if (*pValueSize < sizeof(int16_t)) {
            ALOGV("\tLVM_ERROR : Equalizer_getParameter() invalid pValueSize 1  %d", *pValueSize);
 return -EINVAL;
 }
 *pValueSize = sizeof(int16_t);
 break;

 case EQ_PARAM_LEVEL_RANGE:
 if (*pValueSize < 2 * sizeof(int16_t)) {
            ALOGV("\tLVM_ERROR : Equalizer_getParameter() invalid pValueSize 2  %d", *pValueSize);
 return -EINVAL;
 }
 *pValueSize = 2 * sizeof(int16_t);
 break;
 case EQ_PARAM_BAND_FREQ_RANGE:
 if (*pValueSize < 2 * sizeof(int32_t)) {
            ALOGV("\tLVM_ERROR : Equalizer_getParameter() invalid pValueSize 3  %d", *pValueSize);
 return -EINVAL;
 }
 *pValueSize = 2 * sizeof(int32_t);
 break;

 case EQ_PARAM_CENTER_FREQ:
 if (*pValueSize < sizeof(int32_t)) {
            ALOGV("\tLVM_ERROR : Equalizer_getParameter() invalid pValueSize 5  %d", *pValueSize);
 return -EINVAL;
 }
 *pValueSize = sizeof(int32_t);
 break;

 case EQ_PARAM_GET_PRESET_NAME:
 break;

 case EQ_PARAM_PROPERTIES:
 if (*pValueSize < (2 + FIVEBAND_NUMBANDS) * sizeof(uint16_t)) {
            ALOGV("\tLVM_ERROR : Equalizer_getParameter() invalid pValueSize 1  %d", *pValueSize);
 return -EINVAL;
 }
 *pValueSize = (2 + FIVEBAND_NUMBANDS) * sizeof(uint16_t);
 break;

 default:
        ALOGV("\tLVM_ERROR : Equalizer_getParameter unknown param %d", param);
 return -EINVAL;
 }

 switch (param) {
 case EQ_PARAM_NUM_BANDS:
 *(uint16_t *)pValue = (uint16_t)FIVEBAND_NUMBANDS;
 break;

 case EQ_PARAM_LEVEL_RANGE:
 *(int16_t *)pValue = -1500;
 *((int16_t *)pValue + 1) = 1500;
 break;

 
     case EQ_PARAM_BAND_LEVEL:
         param2 = *pParamTemp;
        if (param2 < 0 || param2 >= FIVEBAND_NUMBANDS) {
             status = -EINVAL;
            if (param2 < 0) {
                android_errorWriteLog(0x534e4554, "32438598");
                ALOGW("\tERROR Equalizer_getParameter() EQ_PARAM_BAND_LEVEL band %d", param2);
            }
             break;
         }
         *(int16_t *)pValue = (int16_t)EqualizerGetBandLevel(pContext, param2);
 break;

 
     case EQ_PARAM_CENTER_FREQ:
         param2 = *pParamTemp;
        if (param2 < 0 || param2 >= FIVEBAND_NUMBANDS) {
             status = -EINVAL;
            if (param2 < 0) {
                android_errorWriteLog(0x534e4554, "32436341");
                ALOGW("\tERROR Equalizer_getParameter() EQ_PARAM_CENTER_FREQ band %d", param2);
            }
             break;
         }
         *(int32_t *)pValue = EqualizerGetCentreFrequency(pContext, param2);
 break;

 
     case EQ_PARAM_BAND_FREQ_RANGE:
         param2 = *pParamTemp;
        if (param2 < 0 || param2 >= FIVEBAND_NUMBANDS) {
             status = -EINVAL;
            if (param2 < 0) {
                android_errorWriteLog(0x534e4554, "32247948");
                ALOGW("\tERROR Equalizer_getParameter() EQ_PARAM_BAND_FREQ_RANGE band %d", param2);
            }
             break;
         }
         EqualizerGetBandFreqRange(pContext, param2, (uint32_t *)pValue, ((uint32_t *)pValue + 1));
 break;

 case EQ_PARAM_GET_BAND:
        param2 = *pParamTemp;
 *(uint16_t *)pValue = (uint16_t)EqualizerGetBand(pContext, param2);
 break;

 case EQ_PARAM_CUR_PRESET:
 *(uint16_t *)pValue = (uint16_t)EqualizerGetPreset(pContext);
 break;

 case EQ_PARAM_GET_NUM_OF_PRESETS:
 *(uint16_t *)pValue = (uint16_t)EqualizerGetNumPresets();
 break;

 case EQ_PARAM_GET_PRESET_NAME:
        param2 = *pParamTemp;
 if (param2 >= EqualizerGetNumPresets()) {
            status = -EINVAL;
 break;
 }
        name = (char *)pValue;
        strncpy(name, EqualizerGetPresetName(param2), *pValueSize - 1);
        name[*pValueSize - 1] = 0;
 *pValueSize = strlen(name) + 1;
 break;

 case EQ_PARAM_PROPERTIES: {
 int16_t *p = (int16_t *)pValue;
        ALOGV("\tEqualizer_getParameter() EQ_PARAM_PROPERTIES");
        p[0] = (int16_t)EqualizerGetPreset(pContext);
        p[1] = (int16_t)FIVEBAND_NUMBANDS;
 for (int i = 0; i < FIVEBAND_NUMBANDS; i++) {
            p[2 + i] = (int16_t)EqualizerGetBandLevel(pContext, i);
 }
 } break;

 default:
        ALOGV("\tLVM_ERROR : Equalizer_getParameter() invalid param %d", param);
        status = -EINVAL;
 break;
 }

 return status;
} /* end Equalizer_getParameter */

int Equalizer_setParameter (EffectContext *pContext, void *pParam, void *pValue){
 int status = 0;
 int32_t preset;
 int32_t band;
 int32_t level;
 int32_t *pParamTemp = (int32_t *)pParam;
 int32_t param = *pParamTemp++;


 switch (param) {
 case EQ_PARAM_CUR_PRESET:
        preset = (int32_t)(*(uint16_t *)pValue);

 if ((preset >= EqualizerGetNumPresets())||(preset < 0)) {
            status = -EINVAL;
 break;
 }
 EqualizerSetPreset(pContext, preset);
 break;
 case EQ_PARAM_BAND_LEVEL:
        band = *pParamTemp;
        level = (int32_t)(*(int16_t *)pValue);
 if (band >= FIVEBAND_NUMBANDS) {
            status = -EINVAL;
 break;
 }
 EqualizerSetBandLevel(pContext, band, level);
 break;
 case EQ_PARAM_PROPERTIES: {
 int16_t *p = (int16_t *)pValue;
 if ((int)p[0] >= EqualizerGetNumPresets()) {
            status = -EINVAL;
 break;
 }
 if (p[0] >= 0) {
 EqualizerSetPreset(pContext, (int)p[0]);
 } else {
 if ((int)p[1] != FIVEBAND_NUMBANDS) {
                status = -EINVAL;
 break;
 }
 for (int i = 0; i < FIVEBAND_NUMBANDS; i++) {
 EqualizerSetBandLevel(pContext, i, (int)p[2 + i]);
 }
 }
 } break;
 default:
        ALOGV("\tLVM_ERROR : Equalizer_setParameter() invalid param %d", param);
        status = -EINVAL;
 break;
 }

 return status;
} /* end Equalizer_setParameter */


int Volume_getParameter(EffectContext *pContext,
 void *pParam,
 uint32_t *pValueSize,
 void *pValue){
 int status = 0;
 int bMute = 0;
 int32_t *pParamTemp = (int32_t *)pParam;
 int32_t param = *pParamTemp++;;
 char *name;


 switch (param){
 case VOLUME_PARAM_LEVEL:
 case VOLUME_PARAM_MAXLEVEL:
 case VOLUME_PARAM_STEREOPOSITION:
 if (*pValueSize != sizeof(int16_t)){
                ALOGV("\tLVM_ERROR : Volume_getParameter() invalid pValueSize 1  %d", *pValueSize);
 return -EINVAL;
 }
 *pValueSize = sizeof(int16_t);
 break;

 case VOLUME_PARAM_MUTE:
 case VOLUME_PARAM_ENABLESTEREOPOSITION:
 if (*pValueSize < sizeof(int32_t)){
                ALOGV("\tLVM_ERROR : Volume_getParameter() invalid pValueSize 2  %d", *pValueSize);
 return -EINVAL;
 }
 *pValueSize = sizeof(int32_t);
 break;

 default:
            ALOGV("\tLVM_ERROR : Volume_getParameter unknown param %d", param);
 return -EINVAL;
 }

 switch (param){
 case VOLUME_PARAM_LEVEL:
            status = VolumeGetVolumeLevel(pContext, (int16_t *)(pValue));
 break;

 case VOLUME_PARAM_MAXLEVEL:
 *(int16_t *)pValue = 0;
 break;

 case VOLUME_PARAM_STEREOPOSITION:
 VolumeGetStereoPosition(pContext, (int16_t *)pValue);
 break;

 case VOLUME_PARAM_MUTE:
            status = VolumeGetMute(pContext, (uint32_t *)pValue);
            ALOGV("\tVolume_getParameter() VOLUME_PARAM_MUTE Value is %d",
 *(uint32_t *)pValue);
 break;

 case VOLUME_PARAM_ENABLESTEREOPOSITION:
 *(int32_t *)pValue = pContext->pBundledContext->bStereoPositionEnabled;
 break;

 default:
            ALOGV("\tLVM_ERROR : Volume_getParameter() invalid param %d", param);
            status = -EINVAL;
 break;
 }

 return status;
} /* end Volume_getParameter */



int Volume_setParameter (EffectContext *pContext, void *pParam, void *pValue){
 int      status = 0;
 int16_t  level;
 int16_t  position;
 uint32_t mute;
 uint32_t positionEnabled;
 int32_t *pParamTemp = (int32_t *)pParam;
 int32_t param = *pParamTemp++;


 switch (param){
 case VOLUME_PARAM_LEVEL:
            level = *(int16_t *)pValue;
            status = VolumeSetVolumeLevel(pContext, (int16_t)level);
 break;

 case VOLUME_PARAM_MUTE:
            mute = *(uint32_t *)pValue;
            status = VolumeSetMute(pContext, mute);
 break;

 case VOLUME_PARAM_ENABLESTEREOPOSITION:
            positionEnabled = *(uint32_t *)pValue;
            status = VolumeEnableStereoPosition(pContext, positionEnabled);
            status = VolumeSetStereoPosition(pContext, pContext->pBundledContext->positionSaved);
 break;

 case VOLUME_PARAM_STEREOPOSITION:
            position = *(int16_t *)pValue;
            status = VolumeSetStereoPosition(pContext, (int16_t)position);
 break;

 default:
            ALOGV("\tLVM_ERROR : Volume_setParameter() invalid param %d", param);
 break;
 }

 return status;
} /* end Volume_setParameter */

/****************************************************************************************
 * Name : LVC_ToDB_s32Tos16()
 *  Input       : Signed 32-bit integer
 *  Output      : Signed 16-bit integer
 *                  MSB (16) = sign bit
 *                  (15->05) = integer part
 *                  (04->01) = decimal part
 *  Returns     : Db value with respect to full scale
 *  Description :
 *  Remarks     :
 ****************************************************************************************/

LVM_INT16 LVC_ToDB_s32Tos16(LVM_INT32 Lin_fix)
{
    LVM_INT16   db_fix;
    LVM_INT16   Shift;
    LVM_INT16   SmallRemainder;
    LVM_UINT32  Remainder = (LVM_UINT32)Lin_fix;

 /* Count leading bits, 1 cycle in assembly*/
 for (Shift = 0; Shift<32; Shift++)
 {
 if ((Remainder & 0x80000000U)!=0)
 {
 break;
 }
 Remainder = Remainder << 1;
 }

 /*
     * Based on the approximation equation (for Q11.4 format):
     *
     * dB = -96 * Shift + 16 * (8 * Remainder - 2 * Remainder^2)
     */
    db_fix    = (LVM_INT16)(-96 * Shift); /* Six dB steps in Q11.4 format*/
 SmallRemainder = (LVM_INT16)((Remainder & 0x7fffffff) >> 24);
    db_fix = (LVM_INT16)(db_fix + SmallRemainder );
 SmallRemainder = (LVM_INT16)(SmallRemainder * SmallRemainder);
    db_fix = (LVM_INT16)(db_fix - (LVM_INT16)((LVM_UINT16)SmallRemainder >> 9));

 /* Correct for small offset */
    db_fix = (LVM_INT16)(db_fix - 5);

 return db_fix;
}


int Effect_setEnabled(EffectContext *pContext, bool enabled)
{
    ALOGV("\tEffect_setEnabled() type %d, enabled %d", pContext->EffectType, enabled);

 if (enabled) {
 bool tempDisabled = false;
 switch (pContext->EffectType) {
 case LVM_BASS_BOOST:
 if (pContext->pBundledContext->bBassEnabled == LVM_TRUE) {
                     ALOGV("\tEffect_setEnabled() LVM_BASS_BOOST is already enabled");
 return -EINVAL;
 }
 if(pContext->pBundledContext->SamplesToExitCountBb <= 0){
                    pContext->pBundledContext->NumberEffectsEnabled++;
 }
                pContext->pBundledContext->SamplesToExitCountBb =
 (LVM_INT32)(pContext->pBundledContext->SamplesPerSecond*0.1);
                pContext->pBundledContext->bBassEnabled = LVM_TRUE;
                tempDisabled = pContext->pBundledContext->bBassTempDisabled;
 break;
 case LVM_EQUALIZER:
 if (pContext->pBundledContext->bEqualizerEnabled == LVM_TRUE) {
                    ALOGV("\tEffect_setEnabled() LVM_EQUALIZER is already enabled");
 return -EINVAL;
 }
 if(pContext->pBundledContext->SamplesToExitCountEq <= 0){
                    pContext->pBundledContext->NumberEffectsEnabled++;
 }
                pContext->pBundledContext->SamplesToExitCountEq =
 (LVM_INT32)(pContext->pBundledContext->SamplesPerSecond*0.1);
                pContext->pBundledContext->bEqualizerEnabled = LVM_TRUE;
 break;
 case LVM_VIRTUALIZER:
 if (pContext->pBundledContext->bVirtualizerEnabled == LVM_TRUE) {
                    ALOGV("\tEffect_setEnabled() LVM_VIRTUALIZER is already enabled");
 return -EINVAL;
 }
 if(pContext->pBundledContext->SamplesToExitCountVirt <= 0){
                    pContext->pBundledContext->NumberEffectsEnabled++;
 }
                pContext->pBundledContext->SamplesToExitCountVirt =
 (LVM_INT32)(pContext->pBundledContext->SamplesPerSecond*0.1);
                pContext->pBundledContext->bVirtualizerEnabled = LVM_TRUE;
                tempDisabled = pContext->pBundledContext->bVirtualizerTempDisabled;
 break;
 case LVM_VOLUME:
 if (pContext->pBundledContext->bVolumeEnabled == LVM_TRUE) {
                    ALOGV("\tEffect_setEnabled() LVM_VOLUME is already enabled");
 return -EINVAL;
 }
                pContext->pBundledContext->NumberEffectsEnabled++;
                pContext->pBundledContext->bVolumeEnabled = LVM_TRUE;
 break;
 default:
                ALOGV("\tEffect_setEnabled() invalid effect type");
 return -EINVAL;
 }
 if (!tempDisabled) {
 LvmEffect_enable(pContext);
 }
 } else {
 switch (pContext->EffectType) {
 case LVM_BASS_BOOST:
 if (pContext->pBundledContext->bBassEnabled == LVM_FALSE) {
                    ALOGV("\tEffect_setEnabled() LVM_BASS_BOOST is already disabled");
 return -EINVAL;
 }
                pContext->pBundledContext->bBassEnabled = LVM_FALSE;
 break;
 case LVM_EQUALIZER:
 if (pContext->pBundledContext->bEqualizerEnabled == LVM_FALSE) {
                    ALOGV("\tEffect_setEnabled() LVM_EQUALIZER is already disabled");
 return -EINVAL;
 }
                pContext->pBundledContext->bEqualizerEnabled = LVM_FALSE;
 break;
 case LVM_VIRTUALIZER:
 if (pContext->pBundledContext->bVirtualizerEnabled == LVM_FALSE) {
                    ALOGV("\tEffect_setEnabled() LVM_VIRTUALIZER is already disabled");
 return -EINVAL;
 }
                pContext->pBundledContext->bVirtualizerEnabled = LVM_FALSE;
 break;
 case LVM_VOLUME:
 if (pContext->pBundledContext->bVolumeEnabled == LVM_FALSE) {
                    ALOGV("\tEffect_setEnabled() LVM_VOLUME is already disabled");
 return -EINVAL;
 }
                pContext->pBundledContext->bVolumeEnabled = LVM_FALSE;
 break;
 default:
                ALOGV("\tEffect_setEnabled() invalid effect type");
 return -EINVAL;
 }
 LvmEffect_disable(pContext);
 }

 return 0;
}


int16_t LVC_Convert_VolToDb(uint32_t vol){
 int16_t  dB;

    dB = LVC_ToDB_s32Tos16(vol <<7);
    dB = (dB +8)>>4;
    dB = (dB <-96) ? -96 : dB ;

 return dB;
}

} // namespace
