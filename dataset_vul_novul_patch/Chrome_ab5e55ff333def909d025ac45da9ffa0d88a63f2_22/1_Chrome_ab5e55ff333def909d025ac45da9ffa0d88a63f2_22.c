    RTCVoidRequestTask(MockWebRTCPeerConnectionHandler* object, const WebKit::WebRTCVoidRequest& request, bool succeeded)
        : MethodTask<MockWebRTCPeerConnectionHandler>(object)
        , m_request(request)
        , m_succeeded(succeeded)
    {
    }
