void WebPluginImpl::didReceiveResponse(WebURLLoader* loader,
                                       const WebURLResponse& response) {
  static const int kHttpPartialResponseStatusCode = 206;
  static const int kHttpResponseSuccessStatusCode = 200;

  WebPluginResourceClient* client = GetClientFromLoader(loader);
  if (!client)
    return;

  ResponseInfo response_info;
  GetResponseInfo(response, &response_info);

  bool request_is_seekable = true;
  if (client->IsMultiByteResponseExpected()) {
    if (response.httpStatusCode() == kHttpPartialResponseStatusCode) {
      HandleHttpMultipartResponse(response, client);
      return;
    } else if (response.httpStatusCode() == kHttpResponseSuccessStatusCode) {
      if (!ReinitializePluginForResponse(loader)) {
        NOTREACHED();
        return;
      }

      request_is_seekable = false;

      delete client;
      client = NULL;

       for (size_t i = 0; i < clients_.size(); ++i) {
         if (clients_[i].loader.get() == loader) {
           WebPluginResourceClient* resource_client =
              delegate_->CreateResourceClient(clients_[i].id, plugin_url_,
                                              false, 0, NULL);
           clients_[i].client = resource_client;
           client = resource_client;
           break;
        }
      }

      DCHECK(client != NULL);
    }
  }

  loader->setDefersLoading(true);

  client->DidReceiveResponse(
      response_info.mime_type,
      GetAllHeaders(response),
      response_info.expected_length,
      response_info.last_modified,
      request_is_seekable);

  if (WebDevToolsAgent* devtools_agent = GetDevToolsAgent()) {
    ClientInfo* client_info = GetClientInfoFromLoader(loader);
    if (client_info)
      devtools_agent->didReceiveResponse(client_info->id, response);
  }

  const GURL& url = response.url();
  if (url.SchemeIs("http") || url.SchemeIs("https")) {
    if (response.httpStatusCode() < 100 || response.httpStatusCode() >= 400) {
      ClientInfo* client_info = GetClientInfoFromLoader(loader);
      if (client_info) {
        client_info->pending_failure_notification = true;
      }
    }
  }
}
