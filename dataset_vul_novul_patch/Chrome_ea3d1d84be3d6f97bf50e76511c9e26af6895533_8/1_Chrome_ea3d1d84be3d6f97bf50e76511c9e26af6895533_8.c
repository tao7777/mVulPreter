void WebPluginProxy::OnResourceCreated(int resource_id, HANDLE cookie) {
  WebPluginResourceClient* resource_client =
      reinterpret_cast<WebPluginResourceClient*>(cookie);
  if (!resource_client) {
    NOTREACHED();
    return;
  }
   DCHECK(resource_clients_.find(resource_id) == resource_clients_.end());
  resource_clients_[resource_id] = resource_client;
 }
