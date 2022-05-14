 void WebPluginImpl::OnDownloadPluginSrcUrl() {
  HandleURLRequestInternal("GET", false, NULL, 0, NULL, false, false,
                           plugin_url_.spec().c_str(), NULL, false,
                           DOCUMENT_URL);
 }
