 PluginStreamUrl* PluginInstance::CreateStream(unsigned long resource_id,
                                               const GURL& url,
                                               const std::string& mime_type,
                                              int notify_id) {

  bool notify;
  void* notify_data;
  GetNotifyData(notify_id, &notify, &notify_data);
   PluginStreamUrl* stream = new PluginStreamUrl(
      resource_id, url, this, notify, notify_data);
 
   AddStream(stream);
   return stream;
}
