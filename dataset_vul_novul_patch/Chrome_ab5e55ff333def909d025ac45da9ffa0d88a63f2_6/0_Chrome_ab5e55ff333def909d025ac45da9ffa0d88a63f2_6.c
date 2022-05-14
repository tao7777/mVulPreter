PassRefPtr<RTCSessionDescriptionRequestImpl> RTCSessionDescriptionRequestImpl::create(ScriptExecutionContext* context, PassRefPtr<RTCSessionDescriptionCallback> successCallback, PassRefPtr<RTCErrorCallback> errorCallback)
PassRefPtr<RTCSessionDescriptionRequestImpl> RTCSessionDescriptionRequestImpl::create(ScriptExecutionContext* context, PassRefPtr<RTCSessionDescriptionCallback> successCallback, PassRefPtr<RTCErrorCallback> errorCallback, PassRefPtr<RTCPeerConnection> owner)
 {
    RefPtr<RTCSessionDescriptionRequestImpl> request = adoptRef(new RTCSessionDescriptionRequestImpl(context, successCallback, errorCallback, owner));
     request->suspendIfNeeded();
     return request.release();
 }
