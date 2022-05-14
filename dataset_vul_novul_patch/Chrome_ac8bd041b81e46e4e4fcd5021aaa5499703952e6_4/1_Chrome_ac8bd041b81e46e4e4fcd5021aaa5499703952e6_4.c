bool PluginServiceImpl::GetPluginInfo(int render_process_id,
                                      int render_view_id,
                                      ResourceContext* context,
                                      const GURL& url,
                                      const GURL& page_url,
                                      const std::string& mime_type,
                                      bool allow_wildcard,
                                      bool* is_stale,
                                      webkit::WebPluginInfo* info,
                                      std::string* actual_mime_type) {
  std::vector<webkit::WebPluginInfo> plugins;
  std::vector<std::string> mime_types;
  bool stale = GetPluginInfoArray(
      url, mime_type, allow_wildcard, &plugins, &mime_types);
  if (is_stale)
     *is_stale = stale;
 
   for (size_t i = 0; i < plugins.size(); ++i) {
    if (!filter_ || filter_->IsPluginEnabled(render_process_id,
                                             render_view_id,
                                             context,
                                             url,
                                             page_url,
                                             &plugins[i])) {
       *info = plugins[i];
       if (actual_mime_type)
         *actual_mime_type = mime_types[i];
      return true;
    }
  }
  return false;
}
