 status_t AudioFlinger::EffectModule::command(uint32_t cmdCode,
                                              uint32_t cmdSize,
                                              void *pCmdData,
 uint32_t *replySize,
 void *pReplyData)
{
 Mutex::Autolock _l(mLock);
    ALOGVV("command(), cmdCode: %d, mEffectInterface: %p", cmdCode, mEffectInterface);

 if (mState == DESTROYED || mEffectInterface == NULL) {
 return NO_INIT;
 }
 if (mStatus != NO_ERROR) {
 return mStatus;
 }
 if (cmdCode == EFFECT_CMD_GET_PARAM &&
 (*replySize < sizeof(effect_param_t) ||
 ((effect_param_t *)pCmdData)->psize > *replySize - sizeof(effect_param_t))) {

         android_errorWriteLog(0x534e4554, "29251553");
         return -EINVAL;
     }
    if ((cmdCode == EFFECT_CMD_SET_PARAM
            || cmdCode == EFFECT_CMD_SET_PARAM_DEFERRED) &&  // DEFERRED not generally used
        (sizeof(effect_param_t) > cmdSize
            || ((effect_param_t *)pCmdData)->psize > cmdSize
                                                     - sizeof(effect_param_t)
            || ((effect_param_t *)pCmdData)->vsize > cmdSize
                                                     - sizeof(effect_param_t)
                                                     - ((effect_param_t *)pCmdData)->psize
            || roundUpDelta(((effect_param_t *)pCmdData)->psize, (uint32_t)sizeof(int)) >
                                                     cmdSize
                                                     - sizeof(effect_param_t)
                                                     - ((effect_param_t *)pCmdData)->psize
                                                     - ((effect_param_t *)pCmdData)->vsize)) {
        android_errorWriteLog(0x534e4554, "30204301");
        return -EINVAL;
    }
     status_t status = (*mEffectInterface)->command(mEffectInterface,
                                                    cmdCode,
                                                    cmdSize,
                                                   pCmdData,
                                                   replySize,
                                                   pReplyData);
 if (cmdCode != EFFECT_CMD_GET_PARAM && status == NO_ERROR) {
 uint32_t size = (replySize == NULL) ? 0 : *replySize;
 for (size_t i = 1; i < mHandles.size(); i++) {
 EffectHandle *h = mHandles[i];
 if (h != NULL && !h->destroyed_l()) {
                h->commandExecuted(cmdCode, cmdSize, pCmdData, size, pReplyData);
 }
 }
 }
 return status;
}
