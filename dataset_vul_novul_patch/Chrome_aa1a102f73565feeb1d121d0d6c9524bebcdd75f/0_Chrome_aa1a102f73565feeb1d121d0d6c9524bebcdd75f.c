void XMLHttpRequest::abortError()
void XMLHttpRequest::handleDidCancel()
 {
    m_exceptionCode = AbortError;

    handleDidFailGeneric();

    if (!m_async) {
        m_state = DONE;
        return;
     }
    changeState(DONE);

    dispatchEventAndLoadEnd(eventNames().abortEvent);
 }
