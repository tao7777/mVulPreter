PassRefPtr<RTCSessionDescriptionDescriptor> RTCPeerConnectionHandlerChromium::localDescription()
{
    if (!m_webHandler)
        return 0;
    return m_webHandler->localDescription();
}
