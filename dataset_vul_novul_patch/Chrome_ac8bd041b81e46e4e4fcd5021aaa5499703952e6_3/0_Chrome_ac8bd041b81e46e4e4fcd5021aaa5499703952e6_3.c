bool FakePluginServiceFilter::IsPluginEnabled(int render_process_id,
bool FakePluginServiceFilter::IsPluginAvailable(int render_process_id,
                                                int render_view_id,
                                                const void* context,
                                                const GURL& url,
                                                const GURL& policy_url,
                                                webkit::WebPluginInfo* plugin) {
   std::map<FilePath, bool>::iterator it = plugin_state_.find(plugin->path);
   if (it == plugin_state_.end()) {
     ADD_FAILURE() << "No plug-in state for '" << plugin->path.value() << "'";
    return false;
  }
  return it->second;
}
