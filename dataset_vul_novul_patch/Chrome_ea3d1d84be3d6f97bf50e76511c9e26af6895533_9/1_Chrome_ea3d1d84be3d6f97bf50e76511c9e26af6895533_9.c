 WebPluginResourceClient* WebPluginDelegatePepper::CreateResourceClient(
    unsigned long resource_id, const GURL& url, bool notify_needed,
    intptr_t notify_data, intptr_t existing_stream) {
  if (existing_stream) {
    NPAPI::PluginStream* plugin_stream =
        reinterpret_cast<NPAPI::PluginStream*>(existing_stream);
    return plugin_stream->AsResourceClient();
  }
 
  std::string mime_type;
  NPAPI::PluginStreamUrl *stream = instance()->CreateStream(
      resource_id, url, mime_type, notify_needed,
      reinterpret_cast<void*>(notify_data));
  return stream;
 }
