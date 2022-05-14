PassRefPtr<RTCSessionDescriptionDescriptor> RTCPeerConnectionHandlerChromium::remoteDescription()
{
    if (!m_webHandler)
        return 0;
    return m_webHandler->remoteDescription();
}
