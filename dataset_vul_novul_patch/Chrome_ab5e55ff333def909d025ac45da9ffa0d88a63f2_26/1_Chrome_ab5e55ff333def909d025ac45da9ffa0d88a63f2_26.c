void MockWebRTCPeerConnectionHandler::setLocalDescription(const WebRTCVoidRequest& request, const WebRTCSessionDescriptionDescriptor& localDescription)
{
    if (!localDescription.isNull() && localDescription.type() == "offer") {
        m_localDescription = localDescription;
        postTask(new RTCVoidRequestTask(this, request, true));
    } else
        postTask(new RTCVoidRequestTask(this, request, false));
}
