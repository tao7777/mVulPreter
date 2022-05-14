void RTCPeerConnectionHandlerChromium::setRemoteDescription(PassRefPtr<RTCVoidRequest> request, PassRefPtr<RTCSessionDescriptionDescriptor> sessionDescription)
{
    if (!m_webHandler)
        return;
    m_webHandler->setRemoteDescription(request, sessionDescription);
}
