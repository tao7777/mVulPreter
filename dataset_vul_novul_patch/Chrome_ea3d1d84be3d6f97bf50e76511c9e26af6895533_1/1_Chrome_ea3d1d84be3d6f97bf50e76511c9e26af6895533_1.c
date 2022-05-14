 void WebPluginDelegateStub::OnHandleURLRequestReply(
    const PluginMsg_URLRequestReply_Params& params) {
   WebPluginResourceClient* resource_client =
      delegate_->CreateResourceClient(params.resource_id, params.url,
                                      params.notify_needed,
                                      params.notify_data,
                                      params.stream);
  webplugin_->OnResourceCreated(params.resource_id, resource_client);
 }
