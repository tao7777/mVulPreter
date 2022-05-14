 int LELib_Create(const effect_uuid_t *uuid,
                         int32_t sessionId,
                         int32_t ioId,
                          effect_handle_t *pHandle) {
     ALOGV("LELib_Create()");
     int ret;
 int i;

 if (pHandle == NULL || uuid == NULL) {
 return -EINVAL;
 }

 if (memcmp(uuid, &gLEDescriptor.uuid, sizeof(effect_uuid_t)) != 0) {
 return -EINVAL;
 }

 LoudnessEnhancerContext *pContext = new LoudnessEnhancerContext;

    pContext->mItfe = &gLEInterface;
    pContext->mState = LOUDNESS_ENHANCER_STATE_UNINITIALIZED;

    pContext->mCompressor = NULL;
    ret = LE_init(pContext);
 if (ret < 0) {
        ALOGW("LELib_Create() init failed");
 delete pContext;
 return ret;
 }

 *pHandle = (effect_handle_t)pContext;

    pContext->mState = LOUDNESS_ENHANCER_STATE_INITIALIZED;

    ALOGV("  LELib_Create context is %p", pContext);

 return 0;

}
