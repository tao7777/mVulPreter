 RTCPeerConnectionHandlerDummy::RTCPeerConnectionHandlerDummy(RTCPeerConnectionHandlerClient* client)
     : m_client(client)
 {
    ASSERT_UNUSED(m_client, m_client);
 }
