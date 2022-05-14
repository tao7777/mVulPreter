int LE_command(effect_handle_t self, uint32_t cmdCode, uint32_t cmdSize,
 void *pCmdData, uint32_t *replySize, void *pReplyData) {

 LoudnessEnhancerContext * pContext = (LoudnessEnhancerContext *)self;
 int retsize;

 if (pContext == NULL || pContext->mState == LOUDNESS_ENHANCER_STATE_UNINITIALIZED) {
 return -EINVAL;
 }

 switch (cmdCode) {
 case EFFECT_CMD_INIT:
 if (pReplyData == NULL || *replySize != sizeof(int)) {
 return -EINVAL;
 }
 *(int *) pReplyData = LE_init(pContext);

         break;
     case EFFECT_CMD_SET_CONFIG:
         if (pCmdData == NULL || cmdSize != sizeof(effect_config_t)
                || pReplyData == NULL || replySize == NULL || *replySize != sizeof(int)) {
             return -EINVAL;
         }
         *(int *) pReplyData = LE_setConfig(pContext,
 (effect_config_t *) pCmdData);
 break;
 case EFFECT_CMD_GET_CONFIG:
 if (pReplyData == NULL ||
 *replySize != sizeof(effect_config_t)) {
 return -EINVAL;
 }
        LE_getConfig(pContext, (effect_config_t *)pReplyData);
 break;
 case EFFECT_CMD_RESET:

         LE_reset(pContext);
         break;
     case EFFECT_CMD_ENABLE:
        if (pReplyData == NULL || replySize == NULL || *replySize != sizeof(int)) {
             return -EINVAL;
         }
         if (pContext->mState != LOUDNESS_ENHANCER_STATE_INITIALIZED) {
 return -ENOSYS;
 }
        pContext->mState = LOUDNESS_ENHANCER_STATE_ACTIVE;
        ALOGV("EFFECT_CMD_ENABLE() OK");
 *(int *)pReplyData = 0;
 break;
 case EFFECT_CMD_DISABLE:
 if (pReplyData == NULL || *replySize != sizeof(int)) {
 return -EINVAL;
 }
 if (pContext->mState != LOUDNESS_ENHANCER_STATE_ACTIVE) {
 return -ENOSYS;
 }
        pContext->mState = LOUDNESS_ENHANCER_STATE_INITIALIZED;
        ALOGV("EFFECT_CMD_DISABLE() OK");
 *(int *)pReplyData = 0;
 break;

     case EFFECT_CMD_GET_PARAM: {
         if (pCmdData == NULL ||
             cmdSize != (int)(sizeof(effect_param_t) + sizeof(uint32_t)) ||
            pReplyData == NULL || replySize == NULL ||
             *replySize < (int)(sizeof(effect_param_t) + sizeof(uint32_t) + sizeof(uint32_t))) {
             return -EINVAL;
         }
        memcpy(pReplyData, pCmdData, sizeof(effect_param_t) + sizeof(uint32_t));
 effect_param_t *p = (effect_param_t *)pReplyData;
        p->status = 0;
 *replySize = sizeof(effect_param_t) + sizeof(uint32_t);
 if (p->psize != sizeof(uint32_t)) {
            p->status = -EINVAL;
 break;
 }
 switch (*(uint32_t *)p->data) {
 case LOUDNESS_ENHANCER_PARAM_TARGET_GAIN_MB:
            ALOGV("get target gain(mB) = %d", pContext->mTargetGainmB);
 *((int32_t *)p->data + 1) = pContext->mTargetGainmB;
            p->vsize = sizeof(int32_t);
 *replySize += sizeof(int32_t);
 break;
 default:
            p->status = -EINVAL;
 }
 } break;

     case EFFECT_CMD_SET_PARAM: {
         if (pCmdData == NULL ||
             cmdSize != (int)(sizeof(effect_param_t) + sizeof(uint32_t) + sizeof(uint32_t)) ||
            pReplyData == NULL || replySize == NULL || *replySize != sizeof(int32_t)) {
             return -EINVAL;
         }
         *(int32_t *)pReplyData = 0;
 effect_param_t *p = (effect_param_t *)pCmdData;
 if (p->psize != sizeof(uint32_t) || p->vsize != sizeof(uint32_t)) {
 *(int32_t *)pReplyData = -EINVAL;
 break;
 }
 switch (*(uint32_t *)p->data) {
 case LOUDNESS_ENHANCER_PARAM_TARGET_GAIN_MB:
            pContext->mTargetGainmB = *((int32_t *)p->data + 1);
            ALOGV("set target gain(mB) = %d", pContext->mTargetGainmB);
            LE_reset(pContext); // apply parameter update
 break;
 default:
 *(int32_t *)pReplyData = -EINVAL;
 }
 } break;
 case EFFECT_CMD_SET_DEVICE:
 case EFFECT_CMD_SET_VOLUME:
 case EFFECT_CMD_SET_AUDIO_MODE:
 break;

 default:
        ALOGW("LE_command invalid command %d",cmdCode);
 return -EINVAL;
 }

 return 0;
}
