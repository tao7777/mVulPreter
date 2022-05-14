PassRefPtr<RTCSessionDescription> RTCPeerConnection::localDescription(ExceptionCode& ec)
{
    if (m_readyState == ReadyStateClosing || m_readyState == ReadyStateClosed) {
        ec = INVALID_STATE_ERR;
        return 0;
    }
    RefPtr<RTCSessionDescriptionDescriptor> descriptor = m_peerHandler->localDescription();
    if (!descriptor)
        return 0;
    RefPtr<RTCSessionDescription> desc = RTCSessionDescription::create(descriptor.release());
    return desc.release();
}
