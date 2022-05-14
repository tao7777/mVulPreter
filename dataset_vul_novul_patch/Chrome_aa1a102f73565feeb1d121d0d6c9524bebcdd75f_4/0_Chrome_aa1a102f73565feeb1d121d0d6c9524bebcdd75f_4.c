void XMLHttpRequest::networkError()
void XMLHttpRequest::dispatchEventAndLoadEnd(const AtomicString& type)
 {
     if (!m_uploadComplete) {
         m_uploadComplete = true;
         if (m_upload && m_uploadEventsAllowed)
            m_upload->dispatchEventAndLoadEnd(XMLHttpRequestProgressEvent::create(type));
    }
    m_progressEventThrottle.dispatchEventAndLoadEnd(XMLHttpRequestProgressEvent::create(type));
}

void XMLHttpRequest::handleNetworkError()
{
    m_exceptionCode = NetworkError;

    handleDidFailGeneric();

    if (m_async) {
        changeState(DONE);
        dispatchEventAndLoadEnd(eventNames().errorEvent);
    } else {
        m_state = DONE;
     }

     internalAbort();
 }
