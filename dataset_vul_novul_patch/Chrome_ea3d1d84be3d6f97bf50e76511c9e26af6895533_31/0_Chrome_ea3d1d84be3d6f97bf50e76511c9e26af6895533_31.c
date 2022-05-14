void PluginInstance::RequestRead(NPStream* stream, NPByteRange* range_list) {
  std::string range_info = "bytes=";

  while (range_list) {
    range_info += IntToString(range_list->offset);
    range_info += "-";
    range_info += IntToString(range_list->offset + range_list->length - 1);
    range_list = range_list->next;
    if (range_list) {
      range_info += ",";
    }
  }

  if (plugin_data_stream_) {
    if (plugin_data_stream_->stream() == stream) {
      webplugin_->CancelDocumentLoad();
      plugin_data_stream_ = NULL;
    }
  }

  std::vector<scoped_refptr<PluginStream> >::iterator stream_index;
  for (stream_index = open_streams_.begin();
          stream_index != open_streams_.end(); ++stream_index) {
    PluginStream* plugin_stream = *stream_index;
    if (plugin_stream->stream() == stream) {
       plugin_stream->set_seekable(true);
 
      pending_range_requests_[++next_range_request_id_] = plugin_stream;
       webplugin_->InitiateHTTPRangeRequest(
          stream->url, range_info.c_str(), next_range_request_id_);
      return;
     }
   }
  NOTREACHED();
}

void PluginInstance::RequestURL(const char* url,
                                const char* method,
                                const char* target,
                                const char* buf,
                                unsigned int len,
                                bool notify,
                                void* notify_data) {
  int notify_id = 0;
  if (notify) {
    notify_id = ++next_notify_id_;
    pending_requests_[notify_id] = notify_data;
  }

  webplugin_->HandleURLRequest(
      url, method, target, buf, len, notify_id, popups_allowed());
 }
