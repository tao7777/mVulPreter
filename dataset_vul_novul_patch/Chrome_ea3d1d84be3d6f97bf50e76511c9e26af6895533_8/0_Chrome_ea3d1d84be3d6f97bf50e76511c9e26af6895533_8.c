void WebPluginProxy::OnResourceCreated(int resource_id, HANDLE cookie) {
void WebPluginProxy::OnResourceCreated(int resource_id,
                                       WebPluginResourceClient* client) {
   DCHECK(resource_clients_.find(resource_id) == resource_clients_.end());
  resource_clients_[resource_id] = client;
 }
