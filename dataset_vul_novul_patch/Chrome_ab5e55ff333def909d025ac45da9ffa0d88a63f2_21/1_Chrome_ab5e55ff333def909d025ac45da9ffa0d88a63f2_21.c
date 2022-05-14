    RTCSessionDescriptionRequestSuccededTask(MockWebRTCPeerConnectionHandler* object, const WebKit::WebRTCSessionDescriptionRequest& request, const WebKit::WebRTCSessionDescriptionDescriptor& result)
        : MethodTask<MockWebRTCPeerConnectionHandler>(object)
        , m_request(request)
        , m_result(result)
    {
    }
