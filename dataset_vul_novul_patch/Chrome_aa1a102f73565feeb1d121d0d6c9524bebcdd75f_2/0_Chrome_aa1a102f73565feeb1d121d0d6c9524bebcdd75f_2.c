void XMLHttpRequest::didTimeout()
void XMLHttpRequest::handleDidTimeout()
 {
     RefPtr<XMLHttpRequest> protect(this);
     internalAbort();
 
     m_exceptionCode = TimeoutError;
 
    handleDidFailGeneric();

     if (!m_async) {
         m_state = DONE;
         return;
     }
     changeState(DONE);
 
    dispatchEventAndLoadEnd(eventNames().timeoutEvent);
 }
