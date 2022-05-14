static int fx_command(effect_handle_t  self,
 uint32_t            cmdCode,
 uint32_t            cmdSize,
 void *pCmdData,
 uint32_t *replySize,
 void *pReplyData)
{
 struct effect_s *effect = (struct effect_s *)self;

 if (effect == NULL)
 return -EINVAL;


 switch (cmdCode) {
 case EFFECT_CMD_INIT:
 if (pReplyData == NULL || *replySize != sizeof(int))
 return -EINVAL;

 *(int *)pReplyData = 0;
 break;

 case EFFECT_CMD_SET_CONFIG: {
 if (pCmdData    == NULL||
                    cmdSize     != sizeof(effect_config_t)||
                    pReplyData  == NULL||
 *replySize  != sizeof(int)) {
                ALOGV("fx_command() EFFECT_CMD_SET_CONFIG invalid args");
 return -EINVAL;
 }
 *(int *)pReplyData = session_set_config(effect->session, (effect_config_t *)pCmdData);
 if (*(int *)pReplyData != 0)
 break;

 if (effect->state != EFFECT_STATE_ACTIVE)
 *(int *)pReplyData = effect_set_state(effect, EFFECT_STATE_CONFIG);

 } break;

 case EFFECT_CMD_GET_CONFIG:
 if (pReplyData == NULL ||
 *replySize != sizeof(effect_config_t)) {
                ALOGV("fx_command() EFFECT_CMD_GET_CONFIG invalid args");
 return -EINVAL;
 }

            session_get_config(effect->session, (effect_config_t *)pReplyData);
 break;

 case EFFECT_CMD_RESET:
 break;

 case EFFECT_CMD_GET_PARAM: {

             if (pCmdData == NULL ||
                     cmdSize < (int)sizeof(effect_param_t) ||
                     pReplyData == NULL ||
                    *replySize < (int)sizeof(effect_param_t)) {
                 ALOGV("fx_command() EFFECT_CMD_GET_PARAM invalid args");
                 return -EINVAL;
             }
 effect_param_t *p = (effect_param_t *)pCmdData;

            memcpy(pReplyData, pCmdData, sizeof(effect_param_t) + p->psize);
            p = (effect_param_t *)pReplyData;
            p->status = -ENOSYS;

 } break;

 case EFFECT_CMD_SET_PARAM: {
 if (pCmdData == NULL||
                    cmdSize < (int)sizeof(effect_param_t) ||
                    pReplyData == NULL ||
 *replySize != sizeof(int32_t)) {
                ALOGV("fx_command() EFFECT_CMD_SET_PARAM invalid args");
 return -EINVAL;
 }
 effect_param_t *p = (effect_param_t *) pCmdData;

 if (p->psize != sizeof(int32_t)) {
                ALOGV("fx_command() EFFECT_CMD_SET_PARAM invalid param format");
 return -EINVAL;
 }
 *(int *)pReplyData = -ENOSYS;
 } break;

 case EFFECT_CMD_ENABLE:
 if (pReplyData == NULL || *replySize != sizeof(int)) {
                ALOGV("fx_command() EFFECT_CMD_ENABLE invalid args");
 return -EINVAL;
 }
 *(int *)pReplyData = effect_set_state(effect, EFFECT_STATE_ACTIVE);
 break;

 case EFFECT_CMD_DISABLE:
 if (pReplyData == NULL || *replySize != sizeof(int)) {
                ALOGV("fx_command() EFFECT_CMD_DISABLE invalid args");
 return -EINVAL;
 }
 *(int *)pReplyData  = effect_set_state(effect, EFFECT_STATE_CONFIG);
 break;

 case EFFECT_CMD_SET_DEVICE:
 case EFFECT_CMD_SET_INPUT_DEVICE:
 case EFFECT_CMD_SET_VOLUME:
 case EFFECT_CMD_SET_AUDIO_MODE:
 if (pCmdData == NULL ||
                    cmdSize != sizeof(uint32_t)) {
                ALOGV("fx_command() %s invalid args",
                      cmdCode == EFFECT_CMD_SET_DEVICE ? "EFFECT_CMD_SET_DEVICE" :
                      cmdCode == EFFECT_CMD_SET_INPUT_DEVICE ? "EFFECT_CMD_SET_INPUT_DEVICE" :
                      cmdCode == EFFECT_CMD_SET_VOLUME ? "EFFECT_CMD_SET_VOLUME" :
                      cmdCode == EFFECT_CMD_SET_AUDIO_MODE ? "EFFECT_CMD_SET_AUDIO_MODE" :
 "");
 return -EINVAL;
 }
            ALOGV("fx_command() %s value %08x",
                  cmdCode == EFFECT_CMD_SET_DEVICE ? "EFFECT_CMD_SET_DEVICE" :
                  cmdCode == EFFECT_CMD_SET_INPUT_DEVICE ? "EFFECT_CMD_SET_INPUT_DEVICE" :
                  cmdCode == EFFECT_CMD_SET_VOLUME ? "EFFECT_CMD_SET_VOLUME" :
                  cmdCode == EFFECT_CMD_SET_AUDIO_MODE ? "EFFECT_CMD_SET_AUDIO_MODE":
 "",
 *(int *)pCmdData);
 break;

 default:
 return -EINVAL;
 }
 return 0;
}
