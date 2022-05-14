status_t OMXNodeInstance::setParameter(
        OMX_INDEXTYPE index, const void *params, size_t size) {
 Mutex::Autolock autoLock(mLock);

     OMX_INDEXEXTTYPE extIndex = (OMX_INDEXEXTTYPE)index;
     CLOG_CONFIG(setParameter, "%s(%#x), %zu@%p)", asString(extIndex), index, size, params);
 
     OMX_ERRORTYPE err = OMX_SetParameter(
             mHandle, index, const_cast<void *>(params));
     CLOG_IF_ERROR(setParameter, err, "%s(%#x)", asString(extIndex), index);
 return StatusFromOMXError(err);
}
