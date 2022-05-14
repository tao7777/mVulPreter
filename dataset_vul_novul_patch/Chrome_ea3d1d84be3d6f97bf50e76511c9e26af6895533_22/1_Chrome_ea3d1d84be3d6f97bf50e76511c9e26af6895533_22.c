 static NPError GetURLNotify(NPP id,
                             const char* url,
                             const char* target,
                            bool notify,
                            void* notify_data) {
   if (!url)
     return NPERR_INVALID_URL;
 
  bool is_javascript_url = IsJavaScriptUrl(url);
   scoped_refptr<NPAPI::PluginInstance> plugin = FindInstance(id);
  if (plugin.get()) {
    plugin->webplugin()->HandleURLRequest(
        "GET", is_javascript_url, target, 0, 0, false,
        notify, url, reinterpret_cast<intptr_t>(notify_data),
        plugin->popups_allowed());
  } else {
     NOTREACHED();
     return NPERR_GENERIC_ERROR;
   }
   return NPERR_NO_ERROR;
 }
