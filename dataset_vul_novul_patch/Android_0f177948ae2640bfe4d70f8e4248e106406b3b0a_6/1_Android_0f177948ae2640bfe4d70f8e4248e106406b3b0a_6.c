status_t OMXNodeInstance::configureVideoTunnelMode(

         OMX_U32 portIndex, OMX_BOOL tunneled, OMX_U32 audioHwSync,
         native_handle_t **sidebandHandle) {
     Mutex::Autolock autolock(mLock);
     CLOG_CONFIG(configureVideoTunnelMode, "%s:%u tun=%d sync=%u",
             portString(portIndex), portIndex, tunneled, audioHwSync);
 
    OMX_INDEXTYPE index;
    OMX_STRING name = const_cast<OMX_STRING>(
 "OMX.google.android.index.configureVideoTunnelMode");

    OMX_ERRORTYPE err = OMX_GetExtensionIndex(mHandle, name, &index);
 if (err != OMX_ErrorNone) {
        CLOG_ERROR_IF(tunneled, getExtensionIndex, err, "%s", name);
 return StatusFromOMXError(err);
 }

 ConfigureVideoTunnelModeParams tunnelParams;
 InitOMXParams(&tunnelParams);
    tunnelParams.nPortIndex = portIndex;
    tunnelParams.bTunneled = tunneled;
    tunnelParams.nAudioHwSync = audioHwSync;
    err = OMX_SetParameter(mHandle, index, &tunnelParams);
 if (err != OMX_ErrorNone) {
        CLOG_ERROR(setParameter, err, "%s(%#x): %s:%u tun=%d sync=%u", name, index,
                portString(portIndex), portIndex, tunneled, audioHwSync);
 return StatusFromOMXError(err);
 }

    err = OMX_GetParameter(mHandle, index, &tunnelParams);
 if (err != OMX_ErrorNone) {
        CLOG_ERROR(getParameter, err, "%s(%#x): %s:%u tun=%d sync=%u", name, index,
                portString(portIndex), portIndex, tunneled, audioHwSync);
 return StatusFromOMXError(err);
 }
 if (sidebandHandle) {
 *sidebandHandle = (native_handle_t*)tunnelParams.pSidebandWindow;
 }

 return OK;
}
