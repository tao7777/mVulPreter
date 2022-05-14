status_t Camera3Device::createDefaultRequest(int templateId,

         CameraMetadata *request) {
     ATRACE_CALL();
     ALOGV("%s: for template %d", __FUNCTION__, templateId);
     Mutex::Autolock il(mInterfaceLock);
     Mutex::Autolock l(mLock);
 
 switch (mStatus) {
 case STATUS_ERROR:
            CLOGE("Device has encountered a serious error");
 return INVALID_OPERATION;
 case STATUS_UNINITIALIZED:
            CLOGE("Device is not initialized!");
 return INVALID_OPERATION;
 case STATUS_UNCONFIGURED:
 case STATUS_CONFIGURED:
 case STATUS_ACTIVE:
 break;
 default:
            SET_ERR_L("Unexpected status: %d", mStatus);
 return INVALID_OPERATION;
 }

 if (!mRequestTemplateCache[templateId].isEmpty()) {
 *request = mRequestTemplateCache[templateId];
 return OK;
 }

 const camera_metadata_t *rawRequest;
    ATRACE_BEGIN("camera3->construct_default_request_settings");
    rawRequest = mHal3Device->ops->construct_default_request_settings(
        mHal3Device, templateId);
    ATRACE_END();
 if (rawRequest == NULL) {
        ALOGI("%s: template %d is not supported on this camera device",
              __FUNCTION__, templateId);
 return BAD_VALUE;
 }
 *request = rawRequest;
    mRequestTemplateCache[templateId] = rawRequest;

 return OK;
}
