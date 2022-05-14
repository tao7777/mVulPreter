PassRefPtr<RTCSessionDescriptionRequestImpl> RTCSessionDescriptionRequestImpl::create(ScriptExecutionContext* context, PassRefPtr<RTCSessionDescriptionCallback> successCallback, PassRefPtr<RTCErrorCallback> errorCallback)
 {
    RefPtr<RTCSessionDescriptionRequestImpl> request = adoptRef(new RTCSessionDescriptionRequestImpl(context, successCallback, errorCallback));
     request->suspendIfNeeded();
     return request.release();
 }
