 PluginStreamUrl* PluginInstance::CreateStream(unsigned long resource_id,
                                               const GURL& url,
                                               const std::string& mime_type,
                                              bool notify_needed,
                                              void* notify_data) {
   PluginStreamUrl* stream = new PluginStreamUrl(
      resource_id, url, this, notify_needed, notify_data);
 
   AddStream(stream);
   return stream;
}
