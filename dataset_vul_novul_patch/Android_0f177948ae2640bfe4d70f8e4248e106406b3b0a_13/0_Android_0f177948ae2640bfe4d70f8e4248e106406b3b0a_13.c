status_t OMXNodeInstance::setConfig(
        OMX_INDEXTYPE index, const void *params, size_t size) {
 Mutex::Autolock autoLock(mLock);

     OMX_INDEXEXTTYPE extIndex = (OMX_INDEXEXTTYPE)index;
     CLOG_CONFIG(setConfig, "%s(%#x), %zu@%p)", asString(extIndex), index, size, params);
 
    if (isProhibitedIndex_l(index)) {
        android_errorWriteLog(0x534e4554, "29422020");
        return BAD_INDEX;
    }

     OMX_ERRORTYPE err = OMX_SetConfig(
             mHandle, index, const_cast<void *>(params));
     CLOG_IF_ERROR(setConfig, err, "%s(%#x)", asString(extIndex), index);
 return StatusFromOMXError(err);
}
