void WebPluginDelegateProxy::OnHandleURLRequest(
    const PluginHostMsg_URLRequest_Params& params) {
  const char* data = NULL;
  if (params.buffer.size())
    data = &params.buffer[0];

  const char* target = NULL;
   if (params.target.length())
     target = params.target.c_str();
 
  plugin_->HandleURLRequest(params.method.c_str(),
                            params.is_javascript_url, target,
                            static_cast<unsigned int>(params.buffer.size()),
                            data, params.is_file_data, params.notify,
                            params.url.c_str(), params.notify_data,
                            params.popups_allowed);
 }
