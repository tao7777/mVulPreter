void XMLHttpRequest::abortError()
 {
    genericError();
    if (!m_uploadComplete) {
        m_uploadComplete = true;
        if (m_upload && m_uploadEventsAllowed)
            m_upload->dispatchEventAndLoadEnd(XMLHttpRequestProgressEvent::create(eventNames().abortEvent));
     }
    m_progressEventThrottle.dispatchEventAndLoadEnd(XMLHttpRequestProgressEvent::create(eventNames().abortEvent));
 }
