 void WebPluginImpl::OnDownloadPluginSrcUrl() {
  HandleURLRequestInternal(
      plugin_url_.spec().c_str(), "GET", NULL, NULL, 0, 0, false, DOCUMENT_URL);
 }
