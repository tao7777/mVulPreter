 void WebPluginDelegateImpl::DidFinishLoadWithReason(const GURL& url,
                                                     NPReason reason,
                                                    int notify_id) {
   if (quirks_ & PLUGIN_QUIRK_ALWAYS_NOTIFY_SUCCESS &&
       reason == NPRES_NETWORK_ERR) {
     reason = NPRES_DONE;
   }
 
  instance()->DidFinishLoadWithReason(url, reason, notify_id);
 }
