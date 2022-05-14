status_t SoundTriggerHwService::Module::startRecognition(sound_model_handle_t handle,
 const sp<IMemory>& dataMemory)
{
    ALOGV("startRecognition() model handle %d", handle);
 if (!captureHotwordAllowed()) {

         return PERMISSION_DENIED;
     }
 
    if (dataMemory == 0 || dataMemory->pointer() == NULL) {
        ALOGE("startRecognition() dataMemory is 0 or has NULL pointer()");
         return BAD_VALUE;
 
     }

    struct sound_trigger_recognition_config *config =
            (struct sound_trigger_recognition_config *)dataMemory->pointer();

    if (config->data_offset < sizeof(struct sound_trigger_recognition_config) ||
            config->data_size > (UINT_MAX - config->data_offset) ||
            dataMemory->size() < config->data_offset ||
            config->data_size > (dataMemory->size() - config->data_offset)) {
        ALOGE("startRecognition() data_size is too big");
        return BAD_VALUE;
    }

     AutoMutex lock(mLock);
     if (mServiceState == SOUND_TRIGGER_STATE_DISABLED) {
         return INVALID_OPERATION;
 }
    sp<Model> model = getModel(handle);

     if (model == 0) {
         return BAD_VALUE;
     }
 
     if (model->mState == Model::STATE_ACTIVE) {
         return INVALID_OPERATION;
     }
 
 
     config->capture_handle = model->mCaptureIOHandle;
    config->capture_device = model->mCaptureDevice;
 status_t status = mHwDevice->start_recognition(mHwDevice, handle, config,
 SoundTriggerHwService::recognitionCallback,
 this);

 if (status == NO_ERROR) {
        model->mState = Model::STATE_ACTIVE;
        model->mConfig = *config;
 }

 return status;
}
