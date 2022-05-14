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
