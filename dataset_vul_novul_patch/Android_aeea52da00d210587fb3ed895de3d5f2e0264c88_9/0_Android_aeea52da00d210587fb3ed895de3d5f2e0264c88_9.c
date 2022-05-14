int PreProcessingFx_Command(effect_handle_t  self,
 uint32_t            cmdCode,
 uint32_t            cmdSize,
 void *pCmdData,
 uint32_t *replySize,
 void *pReplyData)
{
 preproc_effect_t * effect = (preproc_effect_t *) self;
 int retsize;
 int status;

 if (effect == NULL){
 return -EINVAL;
 }


 switch (cmdCode){
 case EFFECT_CMD_INIT:
 if (pReplyData == NULL || *replySize != sizeof(int)){
 return -EINVAL;
 }
 if (effect->ops->init) {
                effect->ops->init(effect);
 }
 *(int *)pReplyData = 0;
 break;

 case EFFECT_CMD_SET_CONFIG: {
 if (pCmdData    == NULL||
                cmdSize     != sizeof(effect_config_t)||
                pReplyData  == NULL||
 *replySize  != sizeof(int)){
                ALOGV("PreProcessingFx_Command cmdCode Case: "
 "EFFECT_CMD_SET_CONFIG: ERROR");
 return -EINVAL;
 }
#ifdef DUAL_MIC_TEST
 uint32_t enabledMsk = effect->session->enabledMsk;
 if (gDualMicEnabled) {
                effect->session->enabledMsk = 0;
 }
#endif
 *(int *)pReplyData = Session_SetConfig(effect->session, (effect_config_t *)pCmdData);
#ifdef DUAL_MIC_TEST
 if (gDualMicEnabled) {
                effect->session->enabledMsk = enabledMsk;
 }
#endif
 if (*(int *)pReplyData != 0) {
 break;
 }
 if (effect->state != PREPROC_EFFECT_STATE_ACTIVE) {
 *(int *)pReplyData = Effect_SetState(effect, PREPROC_EFFECT_STATE_CONFIG);
 }
 } break;

 case EFFECT_CMD_GET_CONFIG:
 if (pReplyData == NULL ||
 *replySize != sizeof(effect_config_t)) {
                ALOGV("\tLVM_ERROR : PreProcessingFx_Command cmdCode Case: "
 "EFFECT_CMD_GET_CONFIG: ERROR");
 return -EINVAL;
 }

 Session_GetConfig(effect->session, (effect_config_t *)pReplyData);
 break;

 case EFFECT_CMD_SET_CONFIG_REVERSE:
 if (pCmdData == NULL ||
                cmdSize != sizeof(effect_config_t) ||
                pReplyData == NULL ||
 *replySize != sizeof(int)) {
                ALOGV("PreProcessingFx_Command cmdCode Case: "
 "EFFECT_CMD_SET_CONFIG_REVERSE: ERROR");
 return -EINVAL;
 }
 *(int *)pReplyData = Session_SetReverseConfig(effect->session,
 (effect_config_t *)pCmdData);
 if (*(int *)pReplyData != 0) {
 break;
 }
 break;

 case EFFECT_CMD_GET_CONFIG_REVERSE:
 if (pReplyData == NULL ||
 *replySize != sizeof(effect_config_t)){
                ALOGV("PreProcessingFx_Command cmdCode Case: "
 "EFFECT_CMD_GET_CONFIG_REVERSE: ERROR");
 return -EINVAL;
 }
 Session_GetReverseConfig(effect->session, (effect_config_t *)pCmdData);
 break;

 case EFFECT_CMD_RESET:
 if (effect->ops->reset) {
                effect->ops->reset(effect);

             }
             break;
 
        case EFFECT_CMD_GET_PARAM: {
            effect_param_t *p = (effect_param_t *)pCmdData;

            if (pCmdData == NULL || cmdSize < sizeof(effect_param_t) ||
                    cmdSize < (sizeof(effect_param_t) + p->psize) ||
                    pReplyData == NULL || replySize == NULL ||
                    *replySize < (sizeof(effect_param_t) + p->psize)){
                 ALOGV("PreProcessingFx_Command cmdCode Case: "
                         "EFFECT_CMD_GET_PARAM: ERROR");
                 return -EINVAL;
             }
 
             memcpy(pReplyData, pCmdData, sizeof(effect_param_t) + p->psize);
 
            p = (effect_param_t *)pReplyData;

 int voffset = ((p->psize - 1) / sizeof(int32_t) + 1) * sizeof(int32_t);

 if (effect->ops->get_parameter) {
                p->status = effect->ops->get_parameter(effect, p->data,
 &p->vsize,
                                                       p->data + voffset);
 *replySize = sizeof(effect_param_t) + voffset + p->vsize;
 }
 } break;

 
         case EFFECT_CMD_SET_PARAM:{
             if (pCmdData == NULL||
                    cmdSize < sizeof(effect_param_t) ||
                    pReplyData == NULL || replySize == NULL ||
                     *replySize != sizeof(int32_t)){
                 ALOGV("PreProcessingFx_Command cmdCode Case: "
                         "EFFECT_CMD_SET_PARAM: ERROR");
 return -EINVAL;
 }
 effect_param_t *p = (effect_param_t *) pCmdData;

 if (p->psize != sizeof(int32_t)){
                ALOGV("PreProcessingFx_Command cmdCode Case: "
 "EFFECT_CMD_SET_PARAM: ERROR, psize is not sizeof(int32_t)");
 return -EINVAL;
 }
 if (effect->ops->set_parameter) {
 *(int *)pReplyData = effect->ops->set_parameter(effect,
 (void *)p->data,
                                                                p->data + p->psize);
 }

         } break;
 
         case EFFECT_CMD_ENABLE:
            if (pReplyData == NULL || replySize == NULL || *replySize != sizeof(int)){
                 ALOGV("PreProcessingFx_Command cmdCode Case: EFFECT_CMD_ENABLE: ERROR");
                 return -EINVAL;
             }
 *(int *)pReplyData = Effect_SetState(effect, PREPROC_EFFECT_STATE_ACTIVE);

             break;
 
         case EFFECT_CMD_DISABLE:
            if (pReplyData == NULL || replySize == NULL || *replySize != sizeof(int)){
                 ALOGV("PreProcessingFx_Command cmdCode Case: EFFECT_CMD_DISABLE: ERROR");
                 return -EINVAL;
             }
 *(int *)pReplyData  = Effect_SetState(effect, PREPROC_EFFECT_STATE_CONFIG);
 break;

 case EFFECT_CMD_SET_DEVICE:
 case EFFECT_CMD_SET_INPUT_DEVICE:
 if (pCmdData == NULL ||
                cmdSize != sizeof(uint32_t)) {
                ALOGV("PreProcessingFx_Command cmdCode Case: EFFECT_CMD_SET_DEVICE: ERROR");
 return -EINVAL;
 }

 if (effect->ops->set_device) {
                effect->ops->set_device(effect, *(uint32_t *)pCmdData);
 }
 break;

 case EFFECT_CMD_SET_VOLUME:
 case EFFECT_CMD_SET_AUDIO_MODE:
 break;

#ifdef DUAL_MIC_TEST
 case PREPROC_CMD_DUAL_MIC_ENABLE: {
 if (pCmdData == NULL|| cmdSize != sizeof(uint32_t) ||
                    pReplyData == NULL || replySize == NULL) {
                ALOGE("PreProcessingFx_Command cmdCode Case: "
 "PREPROC_CMD_DUAL_MIC_ENABLE: ERROR");
 *replySize = 0;
 return -EINVAL;
 }
            gDualMicEnabled = *(bool *)pCmdData;
 if (gDualMicEnabled) {
                effect->aux_channels_on = sHasAuxChannels[effect->procId];
 } else {
                effect->aux_channels_on = false;
 }
            effect->cur_channel_config = (effect->session->inChannelCount == 1) ?
                    CHANNEL_CFG_MONO : CHANNEL_CFG_STEREO;

            ALOGV("PREPROC_CMD_DUAL_MIC_ENABLE: %s", gDualMicEnabled ? "enabled" : "disabled");
 *replySize = sizeof(int);
 *(int *)pReplyData = 0;
 } break;
 case PREPROC_CMD_DUAL_MIC_PCM_DUMP_START: {
 if (pCmdData == NULL|| pReplyData == NULL || replySize == NULL) {
                ALOGE("PreProcessingFx_Command cmdCode Case: "
 "PREPROC_CMD_DUAL_MIC_PCM_DUMP_START: ERROR");
 *replySize = 0;
 return -EINVAL;
 }
            pthread_mutex_lock(&gPcmDumpLock);
 if (gPcmDumpFh != NULL) {
                fclose(gPcmDumpFh);
                gPcmDumpFh = NULL;
 }
 char *path = strndup((char *)pCmdData, cmdSize);
            gPcmDumpFh = fopen((char *)path, "wb");
            pthread_mutex_unlock(&gPcmDumpLock);
            ALOGV("PREPROC_CMD_DUAL_MIC_PCM_DUMP_START: path %s gPcmDumpFh %p",
                  path, gPcmDumpFh);
            ALOGE_IF(gPcmDumpFh <= 0, "gPcmDumpFh open error %d %s", errno, strerror(errno));
            free(path);
 *replySize = sizeof(int);
 *(int *)pReplyData = 0;
 } break;
 case PREPROC_CMD_DUAL_MIC_PCM_DUMP_STOP: {
 if (pReplyData == NULL || replySize == NULL) {
                ALOGE("PreProcessingFx_Command cmdCode Case: "
 "PREPROC_CMD_DUAL_MIC_PCM_DUMP_STOP: ERROR");
 *replySize = 0;
 return -EINVAL;
 }
            pthread_mutex_lock(&gPcmDumpLock);
 if (gPcmDumpFh != NULL) {
                fclose(gPcmDumpFh);
                gPcmDumpFh = NULL;
 }
            pthread_mutex_unlock(&gPcmDumpLock);
            ALOGV("PREPROC_CMD_DUAL_MIC_PCM_DUMP_STOP");
 *replySize = sizeof(int);
 *(int *)pReplyData = 0;
 } break;

 case EFFECT_CMD_GET_FEATURE_SUPPORTED_CONFIGS: {
 if(!gDualMicEnabled) {
 return -EINVAL;
 }
 if (pCmdData == NULL|| cmdSize != 2 * sizeof(uint32_t) ||
                    pReplyData == NULL || replySize == NULL) {
                ALOGE("PreProcessingFx_Command cmdCode Case: "
 "EFFECT_CMD_GET_FEATURE_SUPPORTED_CONFIGS: ERROR");
 *replySize = 0;
 return -EINVAL;
 }
 if (*(uint32_t *)pCmdData != EFFECT_FEATURE_AUX_CHANNELS ||
 !effect->aux_channels_on) {
                ALOGV("PreProcessingFx_Command feature EFFECT_FEATURE_AUX_CHANNELS not supported by"
 " fx %d", effect->procId);
 *(uint32_t *)pReplyData = -ENOSYS;
 *replySize = sizeof(uint32_t);
 break;
 }
 size_t num_configs = *((uint32_t *)pCmdData + 1);
 if (*replySize < (2 * sizeof(uint32_t) +
                              num_configs * sizeof(channel_config_t))) {
 *replySize = 0;
 return -EINVAL;
 }

 *((uint32_t *)pReplyData + 1) = CHANNEL_CFG_CNT;
 if (num_configs < CHANNEL_CFG_CNT ||
 *replySize < (2 * sizeof(uint32_t) +
                                     CHANNEL_CFG_CNT * sizeof(channel_config_t))) {
 *(uint32_t *)pReplyData = -ENOMEM;
 } else {
                num_configs = CHANNEL_CFG_CNT;
 *(uint32_t *)pReplyData = 0;
 }
            ALOGV("PreProcessingFx_Command EFFECT_CMD_GET_FEATURE_SUPPORTED_CONFIGS num config %d",
                  num_configs);

 *replySize = 2 * sizeof(uint32_t) + num_configs * sizeof(channel_config_t);
 *((uint32_t *)pReplyData + 1) = num_configs;
            memcpy((uint32_t *)pReplyData + 2, &sDualMicConfigs, num_configs * sizeof(channel_config_t));
 } break;
 case EFFECT_CMD_GET_FEATURE_CONFIG:
 if(!gDualMicEnabled) {
 return -EINVAL;
 }
 if (pCmdData == NULL|| cmdSize != sizeof(uint32_t) ||
                    pReplyData == NULL || replySize == NULL ||
 *replySize < sizeof(uint32_t) + sizeof(channel_config_t)) {
                ALOGE("PreProcessingFx_Command cmdCode Case: "
 "EFFECT_CMD_GET_FEATURE_CONFIG: ERROR");
 return -EINVAL;
 }
 if (*(uint32_t *)pCmdData != EFFECT_FEATURE_AUX_CHANNELS || !effect->aux_channels_on) {
 *(uint32_t *)pReplyData = -ENOSYS;
 *replySize = sizeof(uint32_t);
 break;
 }
            ALOGV("PreProcessingFx_Command EFFECT_CMD_GET_FEATURE_CONFIG");
 *(uint32_t *)pReplyData = 0;
 *replySize = sizeof(uint32_t) + sizeof(channel_config_t);
            memcpy((uint32_t *)pReplyData + 1,
 &sDualMicConfigs[effect->cur_channel_config],
 sizeof(channel_config_t));
 break;
 case EFFECT_CMD_SET_FEATURE_CONFIG: {
            ALOGV("PreProcessingFx_Command EFFECT_CMD_SET_FEATURE_CONFIG: "
 "gDualMicEnabled %d effect->aux_channels_on %d",
                  gDualMicEnabled, effect->aux_channels_on);
 if(!gDualMicEnabled) {
 return -EINVAL;
 }
 if (pCmdData == NULL|| cmdSize != (sizeof(uint32_t) + sizeof(channel_config_t)) ||
                    pReplyData == NULL || replySize == NULL ||
 *replySize < sizeof(uint32_t)) {
                ALOGE("PreProcessingFx_Command cmdCode Case: "
 "EFFECT_CMD_SET_FEATURE_CONFIG: ERROR\n"
 "pCmdData %p cmdSize %d pReplyData %p replySize %p *replySize %d",
                        pCmdData, cmdSize, pReplyData, replySize, replySize ? *replySize : -1);
 return -EINVAL;
 }
 *replySize = sizeof(uint32_t);
 if (*(uint32_t *)pCmdData != EFFECT_FEATURE_AUX_CHANNELS || !effect->aux_channels_on) {
 *(uint32_t *)pReplyData = -ENOSYS;
                ALOGV("PreProcessingFx_Command cmdCode Case: "
 "EFFECT_CMD_SET_FEATURE_CONFIG: ERROR\n"
 "CmdData %d effect->aux_channels_on %d",
 *(uint32_t *)pCmdData, effect->aux_channels_on);
 break;
 }
 size_t i;
 for (i = 0; i < CHANNEL_CFG_CNT;i++) {
 if (memcmp((uint32_t *)pCmdData + 1,
 &sDualMicConfigs[i], sizeof(channel_config_t)) == 0) {
 break;
 }
 }
 if (i == CHANNEL_CFG_CNT) {
 *(uint32_t *)pReplyData = -EINVAL;
                ALOGW("PreProcessingFx_Command EFFECT_CMD_SET_FEATURE_CONFIG invalid config"
 "[%08x].[%08x]", *((uint32_t *)pCmdData + 1), *((uint32_t *)pCmdData + 2));
 } else {
                effect->cur_channel_config = i;
 *(uint32_t *)pReplyData = 0;
                ALOGV("PreProcessingFx_Command EFFECT_CMD_SET_FEATURE_CONFIG New config"
 "[%08x].[%08x]", sDualMicConfigs[i].main_channels, sDualMicConfigs[i].aux_channels);
 }
 } break;
#endif
 default:
 return -EINVAL;
 }
 return 0;
}
