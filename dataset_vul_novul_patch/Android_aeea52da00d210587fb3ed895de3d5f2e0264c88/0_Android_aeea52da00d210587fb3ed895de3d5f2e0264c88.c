 int32_t DownmixLib_Create(const effect_uuid_t *uuid,
        int32_t sessionId __unused,
        int32_t ioId __unused,
         effect_handle_t *pHandle) {
     int ret;
     int i;
 downmix_module_t *module;
 const effect_descriptor_t *desc;

    ALOGV("DownmixLib_Create()");

#ifdef DOWNMIX_TEST_CHANNEL_INDEX
    ALOGI("DOWNMIX_TEST_CHANNEL_INDEX: should work:");
 Downmix_testIndexComputation(AUDIO_CHANNEL_OUT_FRONT_LEFT | AUDIO_CHANNEL_OUT_FRONT_RIGHT |
                    AUDIO_CHANNEL_OUT_LOW_FREQUENCY | AUDIO_CHANNEL_OUT_BACK_CENTER);
 Downmix_testIndexComputation(CHANNEL_MASK_QUAD_SIDE | CHANNEL_MASK_QUAD_BACK);
 Downmix_testIndexComputation(CHANNEL_MASK_5POINT1_SIDE | AUDIO_CHANNEL_OUT_BACK_CENTER);
 Downmix_testIndexComputation(CHANNEL_MASK_5POINT1_BACK | AUDIO_CHANNEL_OUT_BACK_CENTER);
    ALOGI("DOWNMIX_TEST_CHANNEL_INDEX: should NOT work:");
 Downmix_testIndexComputation(AUDIO_CHANNEL_OUT_FRONT_LEFT | AUDIO_CHANNEL_OUT_FRONT_RIGHT |
                        AUDIO_CHANNEL_OUT_LOW_FREQUENCY | AUDIO_CHANNEL_OUT_BACK_LEFT);
 Downmix_testIndexComputation(AUDIO_CHANNEL_OUT_FRONT_LEFT | AUDIO_CHANNEL_OUT_FRONT_RIGHT |
                            AUDIO_CHANNEL_OUT_LOW_FREQUENCY | AUDIO_CHANNEL_OUT_SIDE_LEFT);
 Downmix_testIndexComputation(AUDIO_CHANNEL_OUT_FRONT_LEFT |
                        AUDIO_CHANNEL_OUT_BACK_LEFT | AUDIO_CHANNEL_OUT_BACK_RIGHT);
 Downmix_testIndexComputation(AUDIO_CHANNEL_OUT_FRONT_LEFT |
                            AUDIO_CHANNEL_OUT_SIDE_LEFT | AUDIO_CHANNEL_OUT_SIDE_RIGHT);
#endif

 if (pHandle == NULL || uuid == NULL) {
 return -EINVAL;
 }

 for (i = 0 ; i < kNbEffects ; i++) {
        desc = gDescriptors[i];
 if (memcmp(uuid, &desc->uuid, sizeof(effect_uuid_t)) == 0) {
 break;
 }
 }

 if (i == kNbEffects) {
 return -ENOENT;
 }

    module = malloc(sizeof(downmix_module_t));

    module->itfe = &gDownmixInterface;

    module->context.state = DOWNMIX_STATE_UNINITIALIZED;

    ret = Downmix_Init(module);
 if (ret < 0) {
        ALOGW("DownmixLib_Create() init failed");
        free(module);
 return ret;
 }

 *pHandle = (effect_handle_t) module;

    ALOGV("DownmixLib_Create() %p , size %zu", module, sizeof(downmix_module_t));

 return 0;
}
