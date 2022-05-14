void MockWebRTCPeerConnectionHandler::setRemoteDescription(const WebRTCVoidRequest& request, const WebRTCSessionDescriptionDescriptor& remoteDescription)
        postTask(new SuccessCallbackTask(this, request, sessionDescription));
     } else
