void WebPluginDelegateProxy::OnHandleURLRequest(
    const PluginHostMsg_URLRequest_Params& params) {
  const char* data = NULL;
  if (params.buffer.size())
    data = &params.buffer[0];

  const char* target = NULL;
   if (params.target.length())
     target = params.target.c_str();
 
  plugin_->HandleURLRequest(
      params.url.c_str(), params.method.c_str(), target, data,
      static_cast<unsigned int>(params.buffer.size()), params.notify_id,
      params.popups_allowed);
 }
