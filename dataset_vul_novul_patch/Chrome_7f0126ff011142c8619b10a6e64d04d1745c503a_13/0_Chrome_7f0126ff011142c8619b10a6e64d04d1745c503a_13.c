     void serialize(const char* url)
     {
         WebURLRequest urlRequest;
        urlRequest.initialize();
        urlRequest.setURL(KURL(m_baseUrl, url));
        m_webViewImpl->mainFrame()->loadRequest(urlRequest);
        Platform::current()->unitTestSupport()->serveAsynchronousMockedRequests();
        runPendingTasks();
         Platform::current()->unitTestSupport()->serveAsynchronousMockedRequests();
 
        PageSerializer serializer(&m_resources);
         serializer.serialize(m_webViewImpl->mainFrameImpl()->frame()->page());
     }
