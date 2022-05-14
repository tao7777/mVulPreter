int Reverb_command(effect_handle_t  self,
 uint32_t            cmdCode,
 uint32_t            cmdSize,
 void *pCmdData,
 uint32_t *replySize,
 void *pReplyData){
    android::ReverbContext * pContext = (android::ReverbContext *) self;
 int retsize;
    LVREV_ControlParams_st    ActiveParams; /* Current control Parameters */
    LVREV_ReturnStatus_en     LvmStatus=LVREV_SUCCESS; /* Function call status */


 if (pContext == NULL){
        ALOGV("\tLVM_ERROR : Reverb_command ERROR pContext == NULL");
 return -EINVAL;
 }


 switch (cmdCode){
 case EFFECT_CMD_INIT:

 if (pReplyData == NULL || replySize == NULL || *replySize != sizeof(int)){
                ALOGV("\tLVM_ERROR : Reverb_command cmdCode Case: "
 "EFFECT_CMD_INIT: ERROR");
 return -EINVAL;
 }
 *(int *) pReplyData = 0;
 break;

 case EFFECT_CMD_SET_CONFIG:
 if (pCmdData == NULL || cmdSize != sizeof(effect_config_t) ||
                    pReplyData == NULL || replySize == NULL || *replySize != sizeof(int)) {
                ALOGV("\tLVM_ERROR : Reverb_command cmdCode Case: "
 "EFFECT_CMD_SET_CONFIG: ERROR");
 return -EINVAL;
 }
 *(int *) pReplyData = android::Reverb_setConfig(pContext,
 (effect_config_t *) pCmdData);
 break;

 case EFFECT_CMD_GET_CONFIG:
 if (pReplyData == NULL || replySize == NULL || *replySize != sizeof(effect_config_t)) {
                ALOGV("\tLVM_ERROR : Reverb_command cmdCode Case: "
 "EFFECT_CMD_GET_CONFIG: ERROR");
 return -EINVAL;
 }

            android::Reverb_getConfig(pContext, (effect_config_t *)pReplyData);
 break;

 case EFFECT_CMD_RESET:
 Reverb_setConfig(pContext, &pContext->config);
 break;

 case EFFECT_CMD_GET_PARAM:{

             effect_param_t *p = (effect_param_t *)pCmdData;
             if (pCmdData == NULL || cmdSize < sizeof(effect_param_t) ||
                     cmdSize < (sizeof(effect_param_t) + p->psize) ||
                     pReplyData == NULL || replySize == NULL ||
 *replySize < (sizeof(effect_param_t) + p->psize)) {
                ALOGV("\tLVM_ERROR : Reverb_command cmdCode Case: "
 "EFFECT_CMD_GET_PARAM: ERROR");
 return -EINVAL;
 }

            memcpy(pReplyData, pCmdData, sizeof(effect_param_t) + p->psize);

            p = (effect_param_t *)pReplyData;

 int voffset = ((p->psize - 1) / sizeof(int32_t) + 1) * sizeof(int32_t);

            p->status = android::Reverb_getParameter(pContext,
 (void *)p->data,
 (size_t *)&p->vsize,
                                                          p->data + voffset);

 *replySize = sizeof(effect_param_t) + voffset + p->vsize;


 } break;
 case EFFECT_CMD_SET_PARAM:{


 if (pCmdData == NULL || (cmdSize < (sizeof(effect_param_t) + sizeof(int32_t))) ||
                    pReplyData == NULL ||  replySize == NULL || *replySize != sizeof(int32_t)) {
                ALOGV("\tLVM_ERROR : Reverb_command cmdCode Case: "
 "EFFECT_CMD_SET_PARAM: ERROR");
 return -EINVAL;
 }

 effect_param_t *p = (effect_param_t *) pCmdData;

 if (p->psize != sizeof(int32_t)){
                ALOGV("\t4LVM_ERROR : Reverb_command cmdCode Case: "
 "EFFECT_CMD_SET_PARAM: ERROR, psize is not sizeof(int32_t)");
 return -EINVAL;
 }


 *(int *)pReplyData = android::Reverb_setParameter(pContext,
 (void *)p->data,
                                                              p->data + p->psize);
 } break;

 case EFFECT_CMD_ENABLE:

 if (pReplyData == NULL || *replySize != sizeof(int)){
                ALOGV("\tLVM_ERROR : Reverb_command cmdCode Case: "
 "EFFECT_CMD_ENABLE: ERROR");
 return -EINVAL;
 }
 if(pContext->bEnabled == LVM_TRUE){
                 ALOGV("\tLVM_ERROR : Reverb_command cmdCode Case: "
 "EFFECT_CMD_ENABLE: ERROR-Effect is already enabled");
 return -EINVAL;
 }
 *(int *)pReplyData = 0;
            pContext->bEnabled = LVM_TRUE;
 /* Get the current settings */
 LvmStatus = LVREV_GetControlParameters(pContext->hInstance, &ActiveParams);
            LVM_ERROR_CHECK(LvmStatus, "LVREV_GetControlParameters", "EFFECT_CMD_ENABLE")
            pContext->SamplesToExitCount =
 (ActiveParams.T60 * pContext->config.inputCfg.samplingRate)/1000;
            pContext->volumeMode = android::REVERB_VOLUME_FLAT;
 break;
 case EFFECT_CMD_DISABLE:

 if (pReplyData == NULL || *replySize != sizeof(int)){
                ALOGV("\tLVM_ERROR : Reverb_command cmdCode Case: "
 "EFFECT_CMD_DISABLE: ERROR");
 return -EINVAL;
 }
 if(pContext->bEnabled == LVM_FALSE){
                 ALOGV("\tLVM_ERROR : Reverb_command cmdCode Case: "
 "EFFECT_CMD_DISABLE: ERROR-Effect is not yet enabled");
 return -EINVAL;
 }
 *(int *)pReplyData = 0;
            pContext->bEnabled = LVM_FALSE;
 break;

 case EFFECT_CMD_SET_VOLUME:
 if (pCmdData == NULL ||
                cmdSize != 2 * sizeof(uint32_t)) {
                ALOGV("\tLVM_ERROR : Reverb_command cmdCode Case: "
 "EFFECT_CMD_SET_VOLUME: ERROR");
 return -EINVAL;
 }


 if (pReplyData != NULL) { // we have volume control
                pContext->leftVolume = (LVM_INT16)((*(uint32_t *)pCmdData + (1 << 11)) >> 12);
                pContext->rightVolume = (LVM_INT16)((*((uint32_t *)pCmdData + 1) + (1 << 11)) >> 12);
 *(uint32_t *)pReplyData = (1 << 24);
 *((uint32_t *)pReplyData + 1) = (1 << 24);
 if (pContext->volumeMode == android::REVERB_VOLUME_OFF) {
                    pContext->volumeMode = android::REVERB_VOLUME_FLAT;
 }
 } else { // we don't have volume control
                pContext->leftVolume = REVERB_UNIT_VOLUME;
                pContext->rightVolume = REVERB_UNIT_VOLUME;
                pContext->volumeMode = android::REVERB_VOLUME_OFF;
 }
            ALOGV("EFFECT_CMD_SET_VOLUME left %d, right %d mode %d",
                    pContext->leftVolume, pContext->rightVolume,  pContext->volumeMode);
 break;

 case EFFECT_CMD_SET_DEVICE:
 case EFFECT_CMD_SET_AUDIO_MODE:
 break;

 default:
            ALOGV("\tLVM_ERROR : Reverb_command cmdCode Case: "
 "DEFAULT start %d ERROR",cmdCode);
 return -EINVAL;
 }

 return 0;
} /* end Reverb_command */
