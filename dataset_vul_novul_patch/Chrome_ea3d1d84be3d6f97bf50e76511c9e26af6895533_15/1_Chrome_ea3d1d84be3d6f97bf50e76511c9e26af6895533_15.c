  void Initialize(unsigned long resource_id, const GURL& url,
                  bool notify_needed, intptr_t notify_data,
                  intptr_t existing_stream) {
     resource_id_ = resource_id;
    url_ = url;
    notify_needed_ = notify_needed;
    notify_data_ = notify_data;
    PluginMsg_URLRequestReply_Params params;
    params.resource_id = resource_id;
    params.url = url_;
    params.notify_needed = notify_needed_;
    params.notify_data = notify_data_;
    params.stream = existing_stream;
    multibyte_response_expected_ = (existing_stream != 0);
 
    channel_->Send(new PluginMsg_HandleURLRequestReply(instance_id_, params));
   }
