void PluginInstance::DidReceiveManualResponse(const GURL& url,
                                              const std::string& mime_type,
                                              const std::string& headers,
                                              uint32 expected_length,
                                               uint32 last_modified) {
   DCHECK(load_manually_);
 
  plugin_data_stream_ = CreateStream(-1, url, mime_type, 0);
   plugin_data_stream_->DidReceiveResponse(mime_type, headers, expected_length,
                                           last_modified, true);
 }
