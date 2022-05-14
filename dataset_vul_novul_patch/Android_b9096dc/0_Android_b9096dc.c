status_t BnSoundTriggerHwService::onTransact(
 uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
 switch(code) {

         case LIST_MODULES: {
             CHECK_INTERFACE(ISoundTriggerHwService, data, reply);
             unsigned int numModulesReq = data.readInt32();
            if (numModulesReq > MAX_ITEMS_PER_LIST) {
                numModulesReq = MAX_ITEMS_PER_LIST;
            }
             unsigned int numModules = numModulesReq;
             struct sound_trigger_module_descriptor *modules =
                     (struct sound_trigger_module_descriptor *)calloc(numModulesReq,
                                                    sizeof(struct sound_trigger_module_descriptor));
            if (modules == NULL) {
                reply->writeInt32(NO_MEMORY);
                reply->writeInt32(0);
                return NO_ERROR;
            }
             status_t status = listModules(modules, &numModules);
             reply->writeInt32(status);
             reply->writeInt32(numModules);
            ALOGV("LIST_MODULES status %d got numModules %d", status, numModules);

 if (status == NO_ERROR) {
 if (numModulesReq > numModules) {
                    numModulesReq = numModules;
 }
                reply->write(modules,
                             numModulesReq * sizeof(struct sound_trigger_module_descriptor));
 }
            free(modules);
 return NO_ERROR;
 }

 case ATTACH: {
            CHECK_INTERFACE(ISoundTriggerHwService, data, reply);
 sound_trigger_module_handle_t handle;
            data.read(&handle, sizeof(sound_trigger_module_handle_t));
            sp<ISoundTriggerClient> client =
                    interface_cast<ISoundTriggerClient>(data.readStrongBinder());
            sp<ISoundTrigger> module;
 status_t status = attach(handle, client, module);
            reply->writeInt32(status);
 if (module != 0) {
                reply->writeInt32(1);
                reply->writeStrongBinder(IInterface::asBinder(module));
 } else {
                reply->writeInt32(0);
 }
 return NO_ERROR;
 } break;

 case SET_CAPTURE_STATE: {
            CHECK_INTERFACE(ISoundTriggerHwService, data, reply);
            reply->writeInt32(setCaptureState((bool)data.readInt32()));
 return NO_ERROR;
 } break;

 default:
 return BBinder::onTransact(code, data, reply, flags);
 }
}
