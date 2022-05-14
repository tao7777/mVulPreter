  void Initialize(unsigned long resource_id, const GURL& url,
  void Initialize(unsigned long resource_id, const GURL& url, int notify_id) {
     resource_id_ = resource_id;
    channel_->Send(new PluginMsg_HandleURLRequestReply(
        instance_id_, resource_id, url, notify_id));
  }
 
  void InitializeForSeekableStream(unsigned long resource_id,
                                   int range_request_id) {
    resource_id_ = resource_id;
    multibyte_response_expected_ = true;
    channel_->Send(new PluginMsg_HTTPRangeRequestReply(
        instance_id_, resource_id, range_request_id));
   }
