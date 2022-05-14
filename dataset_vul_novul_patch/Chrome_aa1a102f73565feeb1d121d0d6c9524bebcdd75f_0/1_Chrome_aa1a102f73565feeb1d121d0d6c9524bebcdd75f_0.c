 void XMLHttpRequest::didFail(const ResourceError& error)
 {
     if (m_error)
         return;
 
     if (error.isCancellation()) {
        m_exceptionCode = AbortError;
        abortError();
         return;
     }
 
     if (error.isTimeout()) {
        didTimeout();
         return;
     }
 
     if (error.domain() == errorDomainWebKitInternal)
         logConsoleError(scriptExecutionContext(), "XMLHttpRequest cannot load " + error.failingURL() + ". " + error.localizedDescription());
 
    m_exceptionCode = NetworkError;
    networkError();
 }
