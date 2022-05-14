status_t SoundTriggerHwService::Module::loadSoundModel(const sp<IMemory>& modelMemory,
 sound_model_handle_t *handle)
{
    ALOGV("loadSoundModel() handle");
 if (!captureHotwordAllowed()) {
 return PERMISSION_DENIED;
 }

 if (modelMemory == 0 || modelMemory->pointer() == NULL) {
        ALOGE("loadSoundModel() modelMemory is 0 or has NULL pointer()");
 return BAD_VALUE;
 }

     struct sound_trigger_sound_model *sound_model =
             (struct sound_trigger_sound_model *)modelMemory->pointer();
 
    size_t structSize;
    if (sound_model->type == SOUND_MODEL_TYPE_KEYPHRASE) {
        structSize = sizeof(struct sound_trigger_phrase_sound_model);
    } else {
        structSize = sizeof(struct sound_trigger_sound_model);
    }

    if (sound_model->data_offset < structSize ||
           sound_model->data_size > (UINT_MAX - sound_model->data_offset) ||
           modelMemory->size() < sound_model->data_offset ||
           sound_model->data_size > (modelMemory->size() - sound_model->data_offset)) {
        android_errorWriteLog(0x534e4554, "30148546");
        ALOGE("loadSoundModel() data_size is too big");
        return BAD_VALUE;
    }

     AutoMutex lock(mLock);
 
     if (mModels.size() >= mDescriptor.properties.max_sound_models) {
        ALOGW("loadSoundModel(): Not loading, max number of models (%d) would be exceeded",
              mDescriptor.properties.max_sound_models);
 return INVALID_OPERATION;
 }

 status_t status = mHwDevice->load_sound_model(mHwDevice, sound_model,
 SoundTriggerHwService::soundModelCallback,
 this, handle);

 if (status != NO_ERROR) {
 return status;
 }
 audio_session_t session;
 audio_io_handle_t ioHandle;
 audio_devices_t device;

    status = AudioSystem::acquireSoundTriggerSession(&session, &ioHandle, &device);
 if (status != NO_ERROR) {
 return status;
 }

    sp<Model> model = new Model(*handle, session, ioHandle, device, sound_model->type);
    mModels.replaceValueFor(*handle, model);

 return status;
}
