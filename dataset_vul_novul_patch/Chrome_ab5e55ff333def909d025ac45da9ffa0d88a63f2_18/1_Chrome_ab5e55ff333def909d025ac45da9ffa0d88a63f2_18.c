void RTCPeerConnectionHandlerChromium::setLocalDescription(PassRefPtr<RTCVoidRequest> request, PassRefPtr<RTCSessionDescriptionDescriptor> sessionDescription)
{
    if (!m_webHandler)
        return;
    m_webHandler->setLocalDescription(request, sessionDescription);
}
