bool ChromePluginServiceFilter::IsPluginEnabled(
bool ChromePluginServiceFilter::IsPluginAvailable(
     int render_process_id,
     int render_view_id,
     const void* context,
    const GURL& url,
    const GURL& policy_url,
    webkit::WebPluginInfo* plugin) {
  base::AutoLock auto_lock(lock_);
  const ProcessDetails* details = GetProcess(render_process_id);

  if (details) {
    for (size_t i = 0; i < details->overridden_plugins.size(); ++i) {
      if (details->overridden_plugins[i].render_view_id == render_view_id &&
          (details->overridden_plugins[i].url == url ||
            details->overridden_plugins[i].url.is_empty())) {
 
         bool use = details->overridden_plugins[i].plugin.path == plugin->path;
        if (use)
          *plugin = details->overridden_plugins[i].plugin;
        return use;
       }
     }
   }

  ResourceContextMap::iterator prefs_it =
      resource_context_map_.find(context);
  if (prefs_it == resource_context_map_.end())
    return false;

  PluginPrefs* plugin_prefs = prefs_it->second.get();
  if (!plugin_prefs->IsPluginEnabled(*plugin))
    return false;

  RestrictedPluginMap::const_iterator it =
      restricted_plugins_.find(plugin->path);
  if (it != restricted_plugins_.end()) {
    if (it->second.first != plugin_prefs)
      return false;
    const GURL& origin = it->second.second;
    if (!origin.is_empty() &&
        (policy_url.scheme() != origin.scheme() ||
         policy_url.host() != origin.host() ||
         policy_url.port() != origin.port())) {
      return false;
    }
  }

  return true;
}
