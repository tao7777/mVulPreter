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
 
       webplugin_->InitiateHTTPRangeRequest(
          stream->url, range_info.c_str(),
          reinterpret_cast<intptr_t>(plugin_stream),
          plugin_stream->notify_needed(),
          reinterpret_cast<intptr_t>(plugin_stream->notify_data()));
      break;
     }
   }
 }
