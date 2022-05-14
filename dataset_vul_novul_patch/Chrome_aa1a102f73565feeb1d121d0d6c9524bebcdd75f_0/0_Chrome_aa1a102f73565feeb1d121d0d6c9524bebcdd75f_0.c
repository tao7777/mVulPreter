 void XMLHttpRequest::didFail(const ResourceError& error)
 {
     if (m_error)
         return;
 
     if (error.isCancellation()) {
        handleDidCancel();
         return;
     }
 
     if (error.isTimeout()) {
        handleDidTimeout();
         return;
     }
 
     if (error.domain() == errorDomainWebKitInternal)
         logConsoleError(scriptExecutionContext(), "XMLHttpRequest cannot load " + error.failingURL() + ". " + error.localizedDescription());
 
    handleNetworkError();
 }
