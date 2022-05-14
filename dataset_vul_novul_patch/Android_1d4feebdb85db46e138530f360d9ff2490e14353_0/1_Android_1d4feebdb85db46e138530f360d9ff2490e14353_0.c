 status_t OMXNodeInstance::enableNativeBuffers(
         OMX_U32 portIndex, OMX_BOOL graphic, OMX_BOOL enable) {
     Mutex::Autolock autoLock(mLock);
     CLOG_CONFIG(enableNativeBuffers, "%s:%u%s, %d", portString(portIndex), portIndex,
                 graphic ? ", graphic" : "", enable);
    OMX_STRING name = const_cast<OMX_STRING>(
            graphic ? "OMX.google.android.index.enableAndroidNativeBuffers"
 : "OMX.google.android.index.allocateNativeHandle");

    OMX_INDEXTYPE index;
    OMX_ERRORTYPE err = OMX_GetExtensionIndex(mHandle, name, &index);

 if (err == OMX_ErrorNone) {
 EnableAndroidNativeBuffersParams params;
 InitOMXParams(&params);
        params.nPortIndex = portIndex;
        params.enable = enable;

        err = OMX_SetParameter(mHandle, index, &params);
        CLOG_IF_ERROR(setParameter, err, "%s(%#x): %s:%u en=%d", name, index,
                      portString(portIndex), portIndex, enable);
 if (!graphic) {
 if (err == OMX_ErrorNone) {
                mSecureBufferType[portIndex] =
                    enable ? kSecureBufferTypeNativeHandle : kSecureBufferTypeOpaque;
 } else if (mSecureBufferType[portIndex] == kSecureBufferTypeUnknown) {
                mSecureBufferType[portIndex] = kSecureBufferTypeOpaque;
 }
 }
 } else {
        CLOG_ERROR_IF(enable, getExtensionIndex, err, "%s", name);
 if (!graphic) {
 char value[PROPERTY_VALUE_MAX];
 if (property_get("media.mediadrmservice.enable", value, NULL)
 && (!strcmp("1", value) || !strcasecmp("true", value))) {
                CLOG_CONFIG(enableNativeBuffers, "system property override: using native-handles");
                mSecureBufferType[portIndex] = kSecureBufferTypeNativeHandle;
 } else if (mSecureBufferType[portIndex] == kSecureBufferTypeUnknown) {
                mSecureBufferType[portIndex] = kSecureBufferTypeOpaque;
 }
            err = OMX_ErrorNone;
 }
 }

 return StatusFromOMXError(err);
}
