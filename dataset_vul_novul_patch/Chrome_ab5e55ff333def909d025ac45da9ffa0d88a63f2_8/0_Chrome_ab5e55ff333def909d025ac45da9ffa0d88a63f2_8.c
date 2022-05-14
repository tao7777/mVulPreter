void RTCSessionDescriptionRequestImpl::requestSucceeded(PassRefPtr<RTCSessionDescriptionDescriptor> descriptor)
 {
     if (m_successCallback) {
         RefPtr<RTCSessionDescription> sessionDescription = RTCSessionDescription::create(descriptor);
        m_successCallback->handleEvent(sessionDescription.get(), m_owner.get());
     }
 
     clear();
}
