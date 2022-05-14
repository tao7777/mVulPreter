 extern "C" int EffectCreate(const effect_uuid_t *uuid,
                            int32_t             sessionId __unused,
                            int32_t             ioId __unused,
                             effect_handle_t  *pHandle){
     int ret;
     int i;
 int length = sizeof(gDescriptors) / sizeof(const effect_descriptor_t *);
 const effect_descriptor_t *desc;

    ALOGV("\t\nEffectCreate start");

 if (pHandle == NULL || uuid == NULL){
        ALOGV("\tLVM_ERROR : EffectCreate() called with NULL pointer");
 return -EINVAL;
 }

 for (i = 0; i < length; i++) {
        desc = gDescriptors[i];
 if (memcmp(uuid, &desc->uuid, sizeof(effect_uuid_t))
 == 0) {
            ALOGV("\tEffectCreate - UUID matched Reverb type %d, UUID = %x", i, desc->uuid.timeLow);
 break;
 }
 }

 if (i == length) {
 return -ENOENT;
 }

 ReverbContext *pContext = new ReverbContext;

    pContext->itfe      = &gReverbInterface;
    pContext->hInstance = NULL;

    pContext->auxiliary = false;
 if ((desc->flags & EFFECT_FLAG_TYPE_MASK) == EFFECT_FLAG_TYPE_AUXILIARY){
        pContext->auxiliary = true;
        ALOGV("\tEffectCreate - AUX");
 }else{
        ALOGV("\tEffectCreate - INS");
 }

    pContext->preset = false;
 if (memcmp(&desc->type, SL_IID_PRESETREVERB, sizeof(effect_uuid_t)) == 0) {
        pContext->preset = true;
        pContext->curPreset = REVERB_PRESET_LAST + 1;
        pContext->nextPreset = REVERB_DEFAULT_PRESET;
        ALOGV("\tEffectCreate - PRESET");
 }else{
        ALOGV("\tEffectCreate - ENVIRONMENTAL");
 }

    ALOGV("\tEffectCreate - Calling Reverb_init");
    ret = Reverb_init(pContext);

 if (ret < 0){
        ALOGV("\tLVM_ERROR : EffectCreate() init failed");
 delete pContext;
 return ret;
 }

 *pHandle = (effect_handle_t)pContext;

 #ifdef LVM_PCM
    pContext->PcmInPtr = NULL;
    pContext->PcmOutPtr = NULL;

    pContext->PcmInPtr = fopen("/data/tmp/reverb_pcm_in.pcm", "w");
    pContext->PcmOutPtr = fopen("/data/tmp/reverb_pcm_out.pcm", "w");

 if((pContext->PcmInPtr == NULL)||
 (pContext->PcmOutPtr == NULL)){
 return -EINVAL;
 }
 #endif


    pContext->InFrames32 = (LVM_INT32 *)malloc(LVREV_MAX_FRAME_SIZE * sizeof(LVM_INT32) * 2);
    pContext->OutFrames32 = (LVM_INT32 *)malloc(LVREV_MAX_FRAME_SIZE * sizeof(LVM_INT32) * 2);

    ALOGV("\tEffectCreate %p, size %zu", pContext, sizeof(ReverbContext));
    ALOGV("\tEffectCreate end\n");
 return 0;
} /* end EffectCreate */
