RTCSessionDescriptionRequestImpl::RTCSessionDescriptionRequestImpl(ScriptExecutionContext* context, PassRefPtr<RTCSessionDescriptionCallback> successCallback, PassRefPtr<RTCErrorCallback> errorCallback)
RTCSessionDescriptionRequestImpl::RTCSessionDescriptionRequestImpl(ScriptExecutionContext* context, PassRefPtr<RTCSessionDescriptionCallback> successCallback, PassRefPtr<RTCErrorCallback> errorCallback, PassRefPtr<RTCPeerConnection> owner)
     : ActiveDOMObject(context, this)
     , m_successCallback(successCallback)
     , m_errorCallback(errorCallback)
    , m_owner(owner)
 {
 }
