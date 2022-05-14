void XMLHttpRequest::didTimeout()
 {
     RefPtr<XMLHttpRequest> protect(this);
     internalAbort();
 
    clearResponse();
    clearRequest();
    m_error = true;
     m_exceptionCode = TimeoutError;
 
     if (!m_async) {
         m_state = DONE;
        m_exceptionCode = TimeoutError;
         return;
     }
     changeState(DONE);
 
    if (!m_uploadComplete) {
        m_uploadComplete = true;
        if (m_upload && m_uploadEventsAllowed)
            m_upload->dispatchEventAndLoadEnd(XMLHttpRequestProgressEvent::create(eventNames().timeoutEvent));
    }
    m_progressEventThrottle.dispatchEventAndLoadEnd(XMLHttpRequestProgressEvent::create(eventNames().timeoutEvent));
 }
