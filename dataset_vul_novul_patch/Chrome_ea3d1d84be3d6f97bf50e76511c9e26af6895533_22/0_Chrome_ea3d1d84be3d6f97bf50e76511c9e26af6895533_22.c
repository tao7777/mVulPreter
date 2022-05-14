 static NPError GetURLNotify(NPP id,
                             const char* url,
                             const char* target,
                            bool notify,
                            void* notify_data) {
   if (!url)
     return NPERR_INVALID_URL;
 
   scoped_refptr<NPAPI::PluginInstance> plugin = FindInstance(id);
  if (!plugin.get()) {
     NOTREACHED();
     return NPERR_GENERIC_ERROR;
   }

  plugin->RequestURL(url, "GET", target, NULL, 0, notify, notify_data);
   return NPERR_NO_ERROR;
 }
