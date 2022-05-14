RTCSessionDescriptionRequestImpl::RTCSessionDescriptionRequestImpl(ScriptExecutionContext* context, PassRefPtr<RTCSessionDescriptionCallback> successCallback, PassRefPtr<RTCErrorCallback> errorCallback)
     : ActiveDOMObject(context, this)
     , m_successCallback(successCallback)
     , m_errorCallback(errorCallback)
 {
 }
