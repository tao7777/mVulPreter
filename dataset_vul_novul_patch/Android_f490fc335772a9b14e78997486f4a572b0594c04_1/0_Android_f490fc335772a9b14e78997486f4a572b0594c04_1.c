int Reverb_setParameter (ReverbContext *pContext, void *pParam, void *pValue){
int Reverb_setParameter (ReverbContext *pContext, void *pParam, void *pValue, int vsize){
     int status = 0;
     int16_t level;
     int16_t ratio;
 uint32_t time;
    t_reverb_settings *pProperties;
 int32_t *pParamTemp = (int32_t *)pParam;
 int32_t param = *pParamTemp++;

 if (pContext->preset) {
 if (param != REVERB_PARAM_PRESET) {
 return -EINVAL;
 }

 uint16_t preset = *(uint16_t *)pValue;
        ALOGV("set REVERB_PARAM_PRESET, preset %d", preset);
 if (preset > REVERB_PRESET_LAST) {
 return -EINVAL;
 }
        pContext->nextPreset = preset;

         return 0;
     }
 
    if (vsize < Reverb_paramValueSize(param)) {
        android_errorWriteLog(0x534e4554, "63526567");
        return -EINVAL;
    }

     switch (param){
         case REVERB_PARAM_PROPERTIES:
             ALOGV("\tReverb_setParameter() REVERB_PARAM_PROPERTIES");
            pProperties = (t_reverb_settings *) pValue;
 ReverbSetRoomLevel(pContext, pProperties->roomLevel);
 ReverbSetRoomHfLevel(pContext, pProperties->roomHFLevel);
 ReverbSetDecayTime(pContext, pProperties->decayTime);
 ReverbSetDecayHfRatio(pContext, pProperties->decayHFRatio);
 ReverbSetReverbLevel(pContext, pProperties->reverbLevel);
 ReverbSetDiffusion(pContext, pProperties->diffusion);
 ReverbSetDensity(pContext, pProperties->density);
 break;
 case REVERB_PARAM_ROOM_LEVEL:
            level = *(int16_t *)pValue;
 ReverbSetRoomLevel(pContext, level);
 break;
 case REVERB_PARAM_ROOM_HF_LEVEL:
            level = *(int16_t *)pValue;
 ReverbSetRoomHfLevel(pContext, level);
 break;
 case REVERB_PARAM_DECAY_TIME:
            time = *(uint32_t *)pValue;
 ReverbSetDecayTime(pContext, time);
 break;
 case REVERB_PARAM_DECAY_HF_RATIO:
            ratio = *(int16_t *)pValue;
 ReverbSetDecayHfRatio(pContext, ratio);
 break;
 case REVERB_PARAM_REVERB_LEVEL:
            level = *(int16_t *)pValue;
 ReverbSetReverbLevel(pContext, level);
 break;
 case REVERB_PARAM_DIFFUSION:
            ratio = *(int16_t *)pValue;
 ReverbSetDiffusion(pContext, ratio);
 break;
 case REVERB_PARAM_DENSITY:
            ratio = *(int16_t *)pValue;
 ReverbSetDensity(pContext, ratio);
 break;
 break;
 case REVERB_PARAM_REFLECTIONS_LEVEL:
 case REVERB_PARAM_REFLECTIONS_DELAY:
 case REVERB_PARAM_REVERB_DELAY:
 break;
 default:
            ALOGV("\tLVM_ERROR : Reverb_setParameter() invalid param %d", param);
 break;
 }


     return status;
 } /* end Reverb_setParameter */
