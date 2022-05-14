 void WebPluginDelegateImpl::DidFinishLoadWithReason(const GURL& url,
                                                     NPReason reason,
                                                    intptr_t notify_data) {
   if (quirks_ & PLUGIN_QUIRK_ALWAYS_NOTIFY_SUCCESS &&
       reason == NPRES_NETWORK_ERR) {
     reason = NPRES_DONE;
   }
 
  instance()->DidFinishLoadWithReason(
      url, reason, reinterpret_cast<void*>(notify_data));
 }
