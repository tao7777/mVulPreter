bool PluginInfoMessageFilter::Context::FindEnabledPlugin(
    int render_view_id,
    const GURL& url,
    const GURL& top_origin_url,
    const std::string& mime_type,
    ChromeViewHostMsg_GetPluginInfo_Status* status,
    WebPluginInfo* plugin,
    std::string* actual_mime_type,
    scoped_ptr<PluginMetadata>* plugin_metadata) const {
  bool allow_wildcard = true;
  std::vector<WebPluginInfo> matching_plugins;
  std::vector<std::string> mime_types;
  PluginService::GetInstance()->GetPluginInfoArray(
      url, mime_type, allow_wildcard, &matching_plugins, &mime_types);
  if (matching_plugins.empty()) {
    status->value = ChromeViewHostMsg_GetPluginInfo_Status::kNotFound;
    return false;
  }

  content::PluginServiceFilter* filter =
       PluginService::GetInstance()->GetFilter();
   size_t i = 0;
   for (; i < matching_plugins.size(); ++i) {
    if (!filter || filter->IsPluginEnabled(render_process_id_,
                                           render_view_id,
                                           resource_context_,
                                           url,
                                           top_origin_url,
                                           &matching_plugins[i])) {
       break;
     }
   }

  bool enabled = i < matching_plugins.size();
  if (!enabled) {
    i = 0;
    status->value = ChromeViewHostMsg_GetPluginInfo_Status::kDisabled;
  }

  *plugin = matching_plugins[i];
  *actual_mime_type = mime_types[i];
  if (plugin_metadata)
    *plugin_metadata = PluginFinder::GetInstance()->GetPluginMetadata(*plugin);

  return enabled;
}
