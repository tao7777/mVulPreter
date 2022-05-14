static int Downmix_Command(effect_handle_t self, uint32_t cmdCode, uint32_t cmdSize,
 void *pCmdData, uint32_t *replySize, void *pReplyData) {

 downmix_module_t *pDwmModule = (downmix_module_t *) self;
 downmix_object_t *pDownmixer;

 if (pDwmModule == NULL || pDwmModule->context.state == DOWNMIX_STATE_UNINITIALIZED) {
 return -EINVAL;
 }

    pDownmixer = (downmix_object_t*) &pDwmModule->context;

    ALOGV("Downmix_Command command %" PRIu32 " cmdSize %" PRIu32, cmdCode, cmdSize);

 switch (cmdCode) {
 case EFFECT_CMD_INIT:
 if (pReplyData == NULL || replySize == NULL || *replySize != sizeof(int)) {
 return -EINVAL;
 }
 *(int *) pReplyData = Downmix_Init(pDwmModule);
 break;

 case EFFECT_CMD_SET_CONFIG:
 if (pCmdData == NULL || cmdSize != sizeof(effect_config_t)
 || pReplyData == NULL || replySize == NULL || *replySize != sizeof(int)) {
 return -EINVAL;
 }
 *(int *) pReplyData = Downmix_Configure(pDwmModule,
 (effect_config_t *)pCmdData, false);
 break;

 case EFFECT_CMD_RESET:
 Downmix_Reset(pDownmixer, false);
 break;

 case EFFECT_CMD_GET_PARAM:
        ALOGV("Downmix_Command EFFECT_CMD_GET_PARAM pCmdData %p, *replySize %" PRIu32 ", pReplyData: %p",
                pCmdData, *replySize, pReplyData);
 if (pCmdData == NULL || cmdSize < (int)(sizeof(effect_param_t) + sizeof(int32_t)) ||
                pReplyData == NULL || replySize == NULL ||
 *replySize < (int) sizeof(effect_param_t) + 2 * sizeof(int32_t)) {
 return -EINVAL;
 }
 effect_param_t *rep = (effect_param_t *) pReplyData;
        memcpy(pReplyData, pCmdData, sizeof(effect_param_t) + sizeof(int32_t));
        ALOGV("Downmix_Command EFFECT_CMD_GET_PARAM param %" PRId32 ", replySize %" PRIu32,
 *(int32_t *)rep->data, rep->vsize);
        rep->status = Downmix_getParameter(pDownmixer, *(int32_t *)rep->data, &rep->vsize,
                rep->data + sizeof(int32_t));
 *replySize = sizeof(effect_param_t) + sizeof(int32_t) + rep->vsize;
 break;

 case EFFECT_CMD_SET_PARAM:
        ALOGV("Downmix_Command EFFECT_CMD_SET_PARAM cmdSize %d pCmdData %p, *replySize %" PRIu32
 ", pReplyData %p", cmdSize, pCmdData, *replySize, pReplyData);
 if (pCmdData == NULL || (cmdSize < (int)(sizeof(effect_param_t) + sizeof(int32_t)))
 || pReplyData == NULL || replySize == NULL || *replySize != (int)sizeof(int32_t)) {

             return -EINVAL;
         }
         effect_param_t *cmd = (effect_param_t *) pCmdData;
        if (cmd->psize != sizeof(int32_t)) {
            android_errorWriteLog(0x534e4554, "63662938");
            return -EINVAL;
        }
         *(int *)pReplyData = Downmix_setParameter(pDownmixer, *(int32_t *)cmd->data,
                 cmd->vsize, cmd->data + sizeof(int32_t));
         break;

 case EFFECT_CMD_SET_PARAM_DEFERRED:
        ALOGW("Downmix_Command command EFFECT_CMD_SET_PARAM_DEFERRED not supported, FIXME");
 break;

 case EFFECT_CMD_SET_PARAM_COMMIT:
        ALOGW("Downmix_Command command EFFECT_CMD_SET_PARAM_COMMIT not supported, FIXME");
 break;

 case EFFECT_CMD_ENABLE:
 if (pReplyData == NULL || replySize == NULL || *replySize != sizeof(int)) {
 return -EINVAL;
 }
 if (pDownmixer->state != DOWNMIX_STATE_INITIALIZED) {
 return -ENOSYS;
 }
        pDownmixer->state = DOWNMIX_STATE_ACTIVE;
        ALOGV("EFFECT_CMD_ENABLE() OK");
 *(int *)pReplyData = 0;
 break;

 case EFFECT_CMD_DISABLE:
 if (pReplyData == NULL || replySize == NULL || *replySize != sizeof(int)) {
 return -EINVAL;
 }
 if (pDownmixer->state != DOWNMIX_STATE_ACTIVE) {
 return -ENOSYS;
 }
        pDownmixer->state = DOWNMIX_STATE_INITIALIZED;
        ALOGV("EFFECT_CMD_DISABLE() OK");
 *(int *)pReplyData = 0;
 break;

 case EFFECT_CMD_SET_DEVICE:
 if (pCmdData == NULL || cmdSize != (int)sizeof(uint32_t)) {
 return -EINVAL;
 }
        ALOGV("Downmix_Command EFFECT_CMD_SET_DEVICE: 0x%08" PRIx32, *(uint32_t *)pCmdData);
 break;

 case EFFECT_CMD_SET_VOLUME: {
 if (pCmdData == NULL || cmdSize != (int)sizeof(uint32_t) * 2) {
 return -EINVAL;
 }
        ALOGW("Downmix_Command command EFFECT_CMD_SET_VOLUME not supported, FIXME");
 float left = (float)(*(uint32_t *)pCmdData) / (1 << 24);
 float right = (float)(*((uint32_t *)pCmdData + 1)) / (1 << 24);
        ALOGV("Downmix_Command EFFECT_CMD_SET_VOLUME: left %f, right %f ", left, right);
 break;
 }

 case EFFECT_CMD_SET_AUDIO_MODE:
 if (pCmdData == NULL || cmdSize != (int)sizeof(uint32_t)) {
 return -EINVAL;
 }
        ALOGV("Downmix_Command EFFECT_CMD_SET_AUDIO_MODE: %" PRIu32, *(uint32_t *)pCmdData);
 break;

 case EFFECT_CMD_SET_CONFIG_REVERSE:
 case EFFECT_CMD_SET_INPUT_DEVICE:
 break;

 default:
        ALOGW("Downmix_Command invalid command %" PRIu32, cmdCode);
 return -EINVAL;
 }

 return 0;
}
