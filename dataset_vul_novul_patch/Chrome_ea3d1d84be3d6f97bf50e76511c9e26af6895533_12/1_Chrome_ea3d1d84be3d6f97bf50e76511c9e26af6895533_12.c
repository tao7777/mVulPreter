 WebPluginDelegateProxy::CreateResourceClient(
    unsigned long resource_id, const GURL& url, bool notify_needed,
    intptr_t notify_data, intptr_t npstream) {
   if (!channel_host_)
     return NULL;
 
   ResourceClientProxy* proxy = new ResourceClientProxy(channel_host_,
                                                        instance_id_);
  proxy->Initialize(resource_id, url, notify_needed, notify_data, npstream);
   return proxy;
 }
