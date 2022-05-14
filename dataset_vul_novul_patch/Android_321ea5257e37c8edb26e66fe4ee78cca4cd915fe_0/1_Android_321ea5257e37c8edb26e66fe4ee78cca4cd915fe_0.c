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

         band =  *pParamTemp;
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
