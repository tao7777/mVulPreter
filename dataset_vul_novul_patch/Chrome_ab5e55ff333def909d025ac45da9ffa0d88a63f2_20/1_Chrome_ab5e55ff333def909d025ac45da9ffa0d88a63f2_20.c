    RTCSessionDescriptionRequestFailedTask(MockWebRTCPeerConnectionHandler* object, const WebKit::WebRTCSessionDescriptionRequest& request)
        : MethodTask<MockWebRTCPeerConnectionHandler>(object)
        , m_request(request)
    {
    }
