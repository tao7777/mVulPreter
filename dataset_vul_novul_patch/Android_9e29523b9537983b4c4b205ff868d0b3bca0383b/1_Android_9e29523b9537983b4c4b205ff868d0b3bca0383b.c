int Effect_command(effect_handle_t  self,
 uint32_t            cmdCode,
 uint32_t            cmdSize,
 void *pCmdData,
 uint32_t *replySize,
 void *pReplyData){
 EffectContext * pContext = (EffectContext *) self;
 int retsize;


 if(pContext->EffectType == LVM_BASS_BOOST){
 }
 if(pContext->EffectType == LVM_VIRTUALIZER){
 }
 if(pContext->EffectType == LVM_EQUALIZER){
 }
 if(pContext->EffectType == LVM_VOLUME){
 }

 if (pContext == NULL){
        ALOGV("\tLVM_ERROR : Effect_command ERROR pContext == NULL");
 return -EINVAL;
 }




 switch (cmdCode){
 case EFFECT_CMD_INIT:
 if (pReplyData == NULL || replySize == NULL || *replySize != sizeof(int)){
                ALOGV("\tLVM_ERROR, EFFECT_CMD_INIT: ERROR for effect type %d",
                        pContext->EffectType);
 return -EINVAL;
 }
 *(int *) pReplyData = 0;
 if(pContext->EffectType == LVM_BASS_BOOST){
                android::BassSetStrength(pContext, 0);
 }
 if(pContext->EffectType == LVM_VIRTUALIZER){
                android::VirtualizerSetStrength(pContext, 0);
 }
 if(pContext->EffectType == LVM_EQUALIZER){
                android::EqualizerSetPreset(pContext, 0);
 }
 if(pContext->EffectType == LVM_VOLUME){
 *(int *) pReplyData = android::VolumeSetVolumeLevel(pContext, 0);
 }
 break;

 case EFFECT_CMD_SET_CONFIG:
 if (pCmdData    == NULL || cmdSize     != sizeof(effect_config_t) ||
                    pReplyData  == NULL || replySize == NULL || *replySize  != sizeof(int)) {
                ALOGV("\tLVM_ERROR : Effect_command cmdCode Case: "
 "EFFECT_CMD_SET_CONFIG: ERROR");
 return -EINVAL;
 }
 *(int *) pReplyData = android::Effect_setConfig(pContext, (effect_config_t *) pCmdData);
 break;

 case EFFECT_CMD_GET_CONFIG:
 if (pReplyData == NULL || replySize == NULL || *replySize != sizeof(effect_config_t)) {
                ALOGV("\tLVM_ERROR : Effect_command cmdCode Case: "
 "EFFECT_CMD_GET_CONFIG: ERROR");
 return -EINVAL;
 }

            android::Effect_getConfig(pContext, (effect_config_t *)pReplyData);
 break;

 case EFFECT_CMD_RESET:
            android::Effect_setConfig(pContext, &pContext->config);
 break;

 case EFFECT_CMD_GET_PARAM:{

 
             effect_param_t *p = (effect_param_t *)pCmdData;
             if (pCmdData == NULL || cmdSize < sizeof(effect_param_t) ||
                     cmdSize < (sizeof(effect_param_t) + p->psize) ||
                     pReplyData == NULL || replySize == NULL ||
 *replySize < (sizeof(effect_param_t) + p->psize)) {
                ALOGV("\tLVM_ERROR : EFFECT_CMD_GET_PARAM: ERROR");
 return -EINVAL;
 }

            memcpy(pReplyData, pCmdData, sizeof(effect_param_t) + p->psize);

            p = (effect_param_t *)pReplyData;

 int voffset = ((p->psize - 1) / sizeof(int32_t) + 1) * sizeof(int32_t);

 if(pContext->EffectType == LVM_BASS_BOOST){
                p->status = android::BassBoost_getParameter(pContext,
                                                            p->data,
 (size_t *)&p->vsize,
                                                            p->data + voffset);
 }

 if(pContext->EffectType == LVM_VIRTUALIZER){
                p->status = android::Virtualizer_getParameter(pContext,
 (void *)p->data,
 (size_t *)&p->vsize,
                                                              p->data + voffset);

 }
 if(pContext->EffectType == LVM_EQUALIZER){
                p->status = android::Equalizer_getParameter(pContext,
                                                            p->data,
 &p->vsize,
                                                            p->data + voffset);

 }
 if(pContext->EffectType == LVM_VOLUME){
                p->status = android::Volume_getParameter(pContext,
 (void *)p->data,
 (size_t *)&p->vsize,
                                                         p->data + voffset);

 }
 *replySize = sizeof(effect_param_t) + voffset + p->vsize;

 } break;
 case EFFECT_CMD_SET_PARAM:{
 if(pContext->EffectType == LVM_BASS_BOOST){

 if (pCmdData   == NULL ||
                        cmdSize    != (sizeof(effect_param_t) + sizeof(int32_t) +sizeof(int16_t)) ||
                        pReplyData == NULL || replySize == NULL || *replySize != sizeof(int32_t)) {
                    ALOGV("\tLVM_ERROR : BassBoost_command cmdCode Case: "
 "EFFECT_CMD_SET_PARAM: ERROR");
 return -EINVAL;
 }
 effect_param_t *p = (effect_param_t *) pCmdData;

 if (p->psize != sizeof(int32_t)){
                    ALOGV("\tLVM_ERROR : BassBoost_command cmdCode Case: "
 "EFFECT_CMD_SET_PARAM: ERROR, psize is not sizeof(int32_t)");
 return -EINVAL;
 }


 *(int *)pReplyData = android::BassBoost_setParameter(pContext,
 (void *)p->data,
                                                                    p->data + p->psize);
 }
 if(pContext->EffectType == LVM_VIRTUALIZER){

 if (pCmdData   == NULL ||
                        cmdSize    > (sizeof(effect_param_t) + sizeof(int32_t) +sizeof(int32_t)) ||
                        cmdSize    < (sizeof(effect_param_t) + sizeof(int32_t) +sizeof(int16_t)) ||
                        pReplyData == NULL || replySize == NULL || *replySize != sizeof(int32_t)) {
                    ALOGV("\tLVM_ERROR : Virtualizer_command cmdCode Case: "
 "EFFECT_CMD_SET_PARAM: ERROR");
 return -EINVAL;
 }
 effect_param_t *p = (effect_param_t *) pCmdData;

 if (p->psize != sizeof(int32_t)){
                    ALOGV("\tLVM_ERROR : Virtualizer_command cmdCode Case: "
 "EFFECT_CMD_SET_PARAM: ERROR, psize is not sizeof(int32_t)");
 return -EINVAL;
 }


 *(int *)pReplyData = android::Virtualizer_setParameter(pContext,
 (void *)p->data,
                                                                       p->data + p->psize);
 }
 if(pContext->EffectType == LVM_EQUALIZER){

 if (pCmdData == NULL || cmdSize < (sizeof(effect_param_t) + sizeof(int32_t)) ||
                        pReplyData == NULL || replySize == NULL || *replySize != sizeof(int32_t)) {
                    ALOGV("\tLVM_ERROR : Equalizer_command cmdCode Case: "
 "EFFECT_CMD_SET_PARAM: ERROR");
 return -EINVAL;
 }
 effect_param_t *p = (effect_param_t *) pCmdData;

 *(int *)pReplyData = android::Equalizer_setParameter(pContext,
 (void *)p->data,
                                                                     p->data + p->psize);
 }
 if(pContext->EffectType == LVM_VOLUME){

 if (pCmdData   == NULL ||
                        cmdSize    < (sizeof(effect_param_t) + sizeof(int32_t)) ||
                        pReplyData == NULL || replySize == NULL ||
 *replySize != sizeof(int32_t)) {
                    ALOGV("\tLVM_ERROR : Volume_command cmdCode Case: "
 "EFFECT_CMD_SET_PARAM: ERROR");
 return -EINVAL;
 }
 effect_param_t *p = (effect_param_t *) pCmdData;

 *(int *)pReplyData = android::Volume_setParameter(pContext,
 (void *)p->data,
                                                                 p->data + p->psize);
 }
 } break;

 case EFFECT_CMD_ENABLE:
            ALOGV("\tEffect_command cmdCode Case: EFFECT_CMD_ENABLE start");
 if (pReplyData == NULL || replySize == NULL || *replySize != sizeof(int)) {
                ALOGV("\tLVM_ERROR : Effect_command cmdCode Case: EFFECT_CMD_ENABLE: ERROR");
 return -EINVAL;
 }

 *(int *)pReplyData = android::Effect_setEnabled(pContext, LVM_TRUE);
 break;

 case EFFECT_CMD_DISABLE:
 if (pReplyData == NULL || replySize == NULL || *replySize != sizeof(int)) {
                ALOGV("\tLVM_ERROR : Effect_command cmdCode Case: EFFECT_CMD_DISABLE: ERROR");
 return -EINVAL;
 }
 *(int *)pReplyData = android::Effect_setEnabled(pContext, LVM_FALSE);
 break;

 case EFFECT_CMD_SET_DEVICE:
 {
            ALOGV("\tEffect_command cmdCode Case: EFFECT_CMD_SET_DEVICE start");
 if (pCmdData   == NULL){
                ALOGV("\tLVM_ERROR : Effect_command cmdCode Case: EFFECT_CMD_SET_DEVICE: ERROR");
 return -EINVAL;
 }

 uint32_t device = *(uint32_t *)pCmdData;

 if (pContext->EffectType == LVM_BASS_BOOST) {
 if((device == AUDIO_DEVICE_OUT_SPEAKER) ||
 (device == AUDIO_DEVICE_OUT_BLUETOOTH_SCO_CARKIT) ||
 (device == AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER)){
                    ALOGV("\tEFFECT_CMD_SET_DEVICE device is invalid for LVM_BASS_BOOST %d",
 *(int32_t *)pCmdData);
                    ALOGV("\tEFFECT_CMD_SET_DEVICE temporary disable LVM_BAS_BOOST");


 if (pContext->pBundledContext->bBassEnabled == LVM_TRUE) {
                        ALOGV("\tEFFECT_CMD_SET_DEVICE disable LVM_BASS_BOOST %d",
 *(int32_t *)pCmdData);
                        android::LvmEffect_disable(pContext);
 }
                    pContext->pBundledContext->bBassTempDisabled = LVM_TRUE;
 } else {
                    ALOGV("\tEFFECT_CMD_SET_DEVICE device is valid for LVM_BASS_BOOST %d",
 *(int32_t *)pCmdData);


 if (pContext->pBundledContext->bBassEnabled == LVM_TRUE) {
                        ALOGV("\tEFFECT_CMD_SET_DEVICE re-enable LVM_BASS_BOOST %d",
 *(int32_t *)pCmdData);
                        android::LvmEffect_enable(pContext);
 }
                    pContext->pBundledContext->bBassTempDisabled = LVM_FALSE;
 }
 }
 if (pContext->EffectType == LVM_VIRTUALIZER) {
 if((device == AUDIO_DEVICE_OUT_SPEAKER)||
 (device == AUDIO_DEVICE_OUT_BLUETOOTH_SCO_CARKIT)||
 (device == AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER)){
                    ALOGV("\tEFFECT_CMD_SET_DEVICE device is invalid for LVM_VIRTUALIZER %d",
 *(int32_t *)pCmdData);
                    ALOGV("\tEFFECT_CMD_SET_DEVICE temporary disable LVM_VIRTUALIZER");


 if (pContext->pBundledContext->bVirtualizerEnabled == LVM_TRUE) {
                        ALOGV("\tEFFECT_CMD_SET_DEVICE disable LVM_VIRTUALIZER %d",
 *(int32_t *)pCmdData);
                        android::LvmEffect_disable(pContext);
 }
                    pContext->pBundledContext->bVirtualizerTempDisabled = LVM_TRUE;
 } else {
                    ALOGV("\tEFFECT_CMD_SET_DEVICE device is valid for LVM_VIRTUALIZER %d",
 *(int32_t *)pCmdData);


 if(pContext->pBundledContext->bVirtualizerEnabled == LVM_TRUE){
                        ALOGV("\tEFFECT_CMD_SET_DEVICE re-enable LVM_VIRTUALIZER %d",
 *(int32_t *)pCmdData);
                        android::LvmEffect_enable(pContext);
 }
                    pContext->pBundledContext->bVirtualizerTempDisabled = LVM_FALSE;
 }
 }
            ALOGV("\tEffect_command cmdCode Case: EFFECT_CMD_SET_DEVICE end");
 break;
 }
 case EFFECT_CMD_SET_VOLUME:
 {
 uint32_t leftVolume, rightVolume;
 int16_t  leftdB, rightdB;
 int16_t  maxdB, pandB;
 int32_t  vol_ret[2] = {1<<24,1<<24}; // Apply no volume
 int      status = 0;
 LVM_ControlParams_t ActiveParams; /* Current control Parameters */
            LVM_ReturnStatus_en     LvmStatus=LVM_SUCCESS; /* Function call status */

 if(pReplyData == LVM_NULL){
 break;
 }

 if (pCmdData == NULL || cmdSize != 2 * sizeof(uint32_t) || pReplyData == NULL ||
                    replySize == NULL || *replySize < 2*sizeof(int32_t)) {
                ALOGV("\tLVM_ERROR : Effect_command cmdCode Case: "
 "EFFECT_CMD_SET_VOLUME: ERROR");
 return -EINVAL;
 }

            leftVolume  = ((*(uint32_t *)pCmdData));
            rightVolume = ((*((uint32_t *)pCmdData + 1)));

 if(leftVolume == 0x1000000){
                leftVolume -= 1;
 }
 if(rightVolume == 0x1000000){
                rightVolume -= 1;
 }

            leftdB  = android::LVC_Convert_VolToDb(leftVolume);
            rightdB = android::LVC_Convert_VolToDb(rightVolume);

            pandB = rightdB - leftdB;

            maxdB = leftdB;
 if(rightdB > maxdB){
                maxdB = rightdB;
 }

            memcpy(pReplyData, vol_ret, sizeof(int32_t)*2);
            android::VolumeSetVolumeLevel(pContext, (int16_t)(maxdB*100));

 /* Get the current settings */
 LvmStatus =LVM_GetControlParameters(pContext->pBundledContext->hInstance,&ActiveParams);
            LVM_ERROR_CHECK(LvmStatus, "LVM_GetControlParameters", "VolumeSetStereoPosition")
 if(LvmStatus != LVM_SUCCESS) return -EINVAL;

 /* Volume parameters */
 ActiveParams.VC_Balance  = pandB;
            ALOGV("\t\tVolumeSetStereoPosition() (-96dB -> +96dB)-> %d\n", ActiveParams.VC_Balance );

 /* Activate the initial settings */
 LvmStatus =LVM_SetControlParameters(pContext->pBundledContext->hInstance,&ActiveParams);
            LVM_ERROR_CHECK(LvmStatus, "LVM_SetControlParameters", "VolumeSetStereoPosition")
 if(LvmStatus != LVM_SUCCESS) return -EINVAL;
 break;
 }
 case EFFECT_CMD_SET_AUDIO_MODE:
 break;
 default:
 return -EINVAL;
 }

 return 0;
} /* end Effect_command */
