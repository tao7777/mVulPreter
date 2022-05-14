int effect_command(effect_handle_t self, uint32_t cmdCode, uint32_t cmdSize,
 void *pCmdData, uint32_t *replySize, void *pReplyData)
{

 effect_context_t * context = (effect_context_t *)self;
 int retsize;
 int status = 0;

    pthread_mutex_lock(&lock);

 if (!effect_exists(context)) {
        status = -ENOSYS;
 goto exit;
 }

 if (context == NULL || context->state == EFFECT_STATE_UNINITIALIZED) {
        status = -ENOSYS;
 goto exit;
 }

 switch (cmdCode) {
 case EFFECT_CMD_INIT:
 if (pReplyData == NULL || *replySize != sizeof(int)) {
            status = -EINVAL;
 goto exit;
 }
 if (context->ops.init)
 *(int *) pReplyData = context->ops.init(context);
 else
 *(int *) pReplyData = 0;
 break;
 case EFFECT_CMD_SET_CONFIG:
 if (pCmdData == NULL || cmdSize != sizeof(effect_config_t)
 || pReplyData == NULL || *replySize != sizeof(int)) {
            status = -EINVAL;
 goto exit;
 }
 *(int *) pReplyData = set_config(context, (effect_config_t *) pCmdData);
 break;
 case EFFECT_CMD_GET_CONFIG:
 if (pReplyData == NULL ||
 *replySize != sizeof(effect_config_t)) {
            status = -EINVAL;
 goto exit;
 }
 if (!context->offload_enabled) {
            status = -EINVAL;
 goto exit;
 }

        get_config(context, (effect_config_t *)pReplyData);
 break;
 case EFFECT_CMD_RESET:
 if (context->ops.reset)
            context->ops.reset(context);
 break;
 case EFFECT_CMD_ENABLE:
 if (pReplyData == NULL || *replySize != sizeof(int)) {
            status = -EINVAL;
 goto exit;
 }
 if (context->state != EFFECT_STATE_INITIALIZED) {
            status = -ENOSYS;
 goto exit;
 }
        context->state = EFFECT_STATE_ACTIVE;
 if (context->ops.enable)
            context->ops.enable(context);
        ALOGV("%s EFFECT_CMD_ENABLE", __func__);
 *(int *)pReplyData = 0;
 break;
 case EFFECT_CMD_DISABLE:
 if (pReplyData == NULL || *replySize != sizeof(int)) {
            status = -EINVAL;
 goto exit;
 }
 if (context->state != EFFECT_STATE_ACTIVE) {
            status = -ENOSYS;
 goto exit;
 }
        context->state = EFFECT_STATE_INITIALIZED;
 if (context->ops.disable)
            context->ops.disable(context);
        ALOGV("%s EFFECT_CMD_DISABLE", __func__);
 *(int *)pReplyData = 0;
 break;
 case EFFECT_CMD_GET_PARAM: {

         if (pCmdData == NULL ||
             cmdSize < (int)(sizeof(effect_param_t) + sizeof(uint32_t)) ||
             pReplyData == NULL ||
            *replySize < (int)(sizeof(effect_param_t) + sizeof(uint32_t) +
                               sizeof(uint16_t))) {
             status = -EINVAL;
             ALOGV("EFFECT_CMD_GET_PARAM invalid command cmdSize %d *replySize %d",
                   cmdSize, *replySize);
 goto exit;
 }
 if (!context->offload_enabled) {
            status = -EINVAL;
 goto exit;
 }
 effect_param_t *q = (effect_param_t *)pCmdData;
        memcpy(pReplyData, pCmdData, sizeof(effect_param_t) + q->psize);
 effect_param_t *p = (effect_param_t *)pReplyData;
 if (context->ops.get_parameter)
            context->ops.get_parameter(context, p, replySize);
 } break;
 case EFFECT_CMD_SET_PARAM: {
 if (pCmdData == NULL ||
            cmdSize < (int)(sizeof(effect_param_t) + sizeof(uint32_t) +
 sizeof(uint16_t)) ||
            pReplyData == NULL || *replySize != sizeof(int32_t)) {
            status = -EINVAL;
            ALOGV("EFFECT_CMD_SET_PARAM invalid command cmdSize %d *replySize %d",
                  cmdSize, *replySize);
 goto exit;
 }
 *(int32_t *)pReplyData = 0;
 effect_param_t *p = (effect_param_t *)pCmdData;
 if (context->ops.set_parameter)
 *(int32_t *)pReplyData = context->ops.set_parameter(context, p,
 *replySize);

 } break;
 case EFFECT_CMD_SET_DEVICE: {
 uint32_t device;
        ALOGV("\t EFFECT_CMD_SET_DEVICE start");
 if (pCmdData == NULL || cmdSize < sizeof(uint32_t)) {
            status = -EINVAL;
            ALOGV("EFFECT_CMD_SET_DEVICE invalid command cmdSize %d", cmdSize);
 goto exit;
 }
        device = *(uint32_t *)pCmdData;
 if (context->ops.set_device)
            context->ops.set_device(context, device);
 } break;
 case EFFECT_CMD_SET_VOLUME:
 case EFFECT_CMD_SET_AUDIO_MODE:
 break;

 case EFFECT_CMD_OFFLOAD: {
 output_context_t *out_ctxt;

 if (cmdSize != sizeof(effect_offload_param_t) || pCmdData == NULL
 || pReplyData == NULL || *replySize != sizeof(int)) {
            ALOGV("%s EFFECT_CMD_OFFLOAD bad format", __func__);
            status = -EINVAL;
 break;
 }

 effect_offload_param_t* offload_param = (effect_offload_param_t*)pCmdData;

        ALOGV("%s EFFECT_CMD_OFFLOAD offload %d output %d", __func__,
              offload_param->isOffload, offload_param->ioHandle);

 *(int *)pReplyData = 0;

        context->offload_enabled = offload_param->isOffload;
 if (context->out_handle == offload_param->ioHandle)
 break;

        out_ctxt = get_output(context->out_handle);
 if (out_ctxt != NULL)
            remove_effect_from_output(out_ctxt, context);

        context->out_handle = offload_param->ioHandle;
        out_ctxt = get_output(context->out_handle);
 if (out_ctxt != NULL)
            add_effect_to_output(out_ctxt, context);

 } break;


 default:
 if (cmdCode >= EFFECT_CMD_FIRST_PROPRIETARY && context->ops.command)
            status = context->ops.command(context, cmdCode, cmdSize,
                                          pCmdData, replySize, pReplyData);
 else {
            ALOGW("%s invalid command %d", __func__, cmdCode);
            status = -EINVAL;
 }
 break;
 }

exit:
    pthread_mutex_unlock(&lock);

 return status;
}
