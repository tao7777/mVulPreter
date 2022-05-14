int equalizer_get_parameter(effect_context_t *context, effect_param_t *p,
 uint32_t *size)
{
 equalizer_context_t *eq_ctxt = (equalizer_context_t *)context;
 int voffset = ((p->psize - 1) / sizeof(int32_t) + 1) * sizeof(int32_t);
 int32_t *param_tmp = (int32_t *)p->data;
 int32_t param = *param_tmp++;
 int32_t param2;
 char *name;
 void *value = p->data + voffset;
 int i;

    ALOGV("%s", __func__);

    p->status = 0;

 switch (param) {
 case EQ_PARAM_NUM_BANDS:
 case EQ_PARAM_CUR_PRESET:
 case EQ_PARAM_GET_NUM_OF_PRESETS:
 case EQ_PARAM_BAND_LEVEL:
 case EQ_PARAM_GET_BAND:
 if (p->vsize < sizeof(int16_t))
           p->status = -EINVAL;
        p->vsize = sizeof(int16_t);
 break;

 case EQ_PARAM_LEVEL_RANGE:
 if (p->vsize < 2 * sizeof(int16_t))
            p->status = -EINVAL;
        p->vsize = 2 * sizeof(int16_t);
 break;
 case EQ_PARAM_BAND_FREQ_RANGE:
 if (p->vsize < 2 * sizeof(int32_t))
            p->status = -EINVAL;
        p->vsize = 2 * sizeof(int32_t);
 break;

 case EQ_PARAM_CENTER_FREQ:
 if (p->vsize < sizeof(int32_t))
            p->status = -EINVAL;
        p->vsize = sizeof(int32_t);
 break;

 case EQ_PARAM_GET_PRESET_NAME:
 break;

 case EQ_PARAM_PROPERTIES:
 if (p->vsize < (2 + NUM_EQ_BANDS) * sizeof(uint16_t))
            p->status = -EINVAL;
        p->vsize = (2 + NUM_EQ_BANDS) * sizeof(uint16_t);
 break;

 default:
        p->status = -EINVAL;
 }

 *size = sizeof(effect_param_t) + voffset + p->vsize;

 if (p->status != 0)
 return 0;

 switch (param) {
 case EQ_PARAM_NUM_BANDS:
	ALOGV("%s: EQ_PARAM_NUM_BANDS", __func__);
 *(uint16_t *)value = (uint16_t)NUM_EQ_BANDS;
 break;

 case EQ_PARAM_LEVEL_RANGE:
	ALOGV("%s: EQ_PARAM_LEVEL_RANGE", __func__);
 *(int16_t *)value = -1500;
 *((int16_t *)value + 1) = 1500;
 break;


     case EQ_PARAM_BAND_LEVEL:
 	ALOGV("%s: EQ_PARAM_BAND_LEVEL", __func__);
         param2 = *param_tmp;
        if (param2 < 0 || param2 >= NUM_EQ_BANDS) {
             p->status = -EINVAL;
            if (param2 < 0) {
                android_errorWriteLog(0x534e4554, "32438598");
                ALOGW("\tERROR EQ_PARAM_BAND_LEVEL band %d", param2);
            }
             break;
         }
         *(int16_t *)value = (int16_t)equalizer_get_band_level(eq_ctxt, param2);
 break;


     case EQ_PARAM_CENTER_FREQ:
 	ALOGV("%s: EQ_PARAM_CENTER_FREQ", __func__);
         param2 = *param_tmp;
        if (param2 < 0 || param2 >= NUM_EQ_BANDS) {
            p->status = -EINVAL;
            if (param2 < 0) {
                android_errorWriteLog(0x534e4554, "32436341");
                ALOGW("\tERROR EQ_PARAM_CENTER_FREQ band %d", param2);
            }
             break;
         }
         *(int32_t *)value = equalizer_get_center_frequency(eq_ctxt, param2);
 break;


     case EQ_PARAM_BAND_FREQ_RANGE:
 	ALOGV("%s: EQ_PARAM_BAND_FREQ_RANGE", __func__);
         param2 = *param_tmp;
        if (param2 < 0 || param2 >= NUM_EQ_BANDS) {
             p->status = -EINVAL;
            if (param2 < 0) {
                android_errorWriteLog(0x534e4554, "32247948");
                ALOGW("\tERROR EQ_PARAM_BAND_FREQ_RANGE band %d", param2);
            }
            break;
         }
        equalizer_get_band_freq_range(eq_ctxt, param2, (uint32_t *)value,
 ((uint32_t *)value + 1));
 break;

 case EQ_PARAM_GET_BAND:
	ALOGV("%s: EQ_PARAM_GET_BAND", __func__);
        param2 = *param_tmp;
 *(uint16_t *)value = (uint16_t)equalizer_get_band(eq_ctxt, param2);
 break;

 case EQ_PARAM_CUR_PRESET:
	ALOGV("%s: EQ_PARAM_CUR_PRESET", __func__);
 *(uint16_t *)value = (uint16_t)equalizer_get_preset(eq_ctxt);
 break;

 case EQ_PARAM_GET_NUM_OF_PRESETS:
	ALOGV("%s: EQ_PARAM_GET_NUM_OF_PRESETS", __func__);
 *(uint16_t *)value = (uint16_t)equalizer_get_num_presets(eq_ctxt);
 break;

 case EQ_PARAM_GET_PRESET_NAME:
	ALOGV("%s: EQ_PARAM_GET_PRESET_NAME", __func__);
        param2 = *param_tmp;
	ALOGV("param2: %d", param2);
 if (param2 >= equalizer_get_num_presets(eq_ctxt)) {
            p->status = -EINVAL;
 break;
 }
        name = (char *)value;
        strlcpy(name, equalizer_get_preset_name(eq_ctxt, param2), p->vsize - 1);
        name[p->vsize - 1] = 0;
        p->vsize = strlen(name) + 1;
 break;

 case EQ_PARAM_PROPERTIES: {
	ALOGV("%s: EQ_PARAM_PROPERTIES", __func__);
 int16_t *prop = (int16_t *)value;
        prop[0] = (int16_t)equalizer_get_preset(eq_ctxt);
        prop[1] = (int16_t)NUM_EQ_BANDS;
 for (i = 0; i < NUM_EQ_BANDS; i++) {
            prop[2 + i] = (int16_t)equalizer_get_band_level(eq_ctxt, i);
 }
 } break;

 default:
        p->status = -EINVAL;
 break;
 }

 return 0;
}
