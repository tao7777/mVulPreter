void RTCPeerConnection::createOffer(PassRefPtr<RTCSessionDescriptionCallback> successCallback, PassRefPtr<RTCErrorCallback> errorCallback, const Dictionary& mediaConstraints, ExceptionCode& ec)
{
    if (m_readyState == ReadyStateClosing || m_readyState == ReadyStateClosed) {
        ec = INVALID_STATE_ERR;
        return;
    }

    if (!successCallback) {
        ec = TYPE_MISMATCH_ERR;
        return;
    }

    RefPtr<MediaConstraints> constraints = MediaConstraintsImpl::create(mediaConstraints, ec);
     if (ec)
         return;
 
    RefPtr<RTCSessionDescriptionRequestImpl> request = RTCSessionDescriptionRequestImpl::create(scriptExecutionContext(), successCallback, errorCallback, this);
     m_peerHandler->createOffer(request.release(), constraints);
 }
