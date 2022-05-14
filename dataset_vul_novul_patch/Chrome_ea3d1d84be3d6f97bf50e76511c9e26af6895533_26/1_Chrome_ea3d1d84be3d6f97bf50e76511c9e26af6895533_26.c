void PluginInstance::DidFinishLoadWithReason(const GURL& url, NPReason reason,
                                             void* notify_data) {
   NPP_URLNotify(url.spec().c_str(), reason, notify_data);
 }
