void DevToolsAgentHostImpl::ForceDetachAllClients() {
void DevToolsAgentHostImpl::ForceDetachAllSessions() {
   scoped_refptr<DevToolsAgentHostImpl> protect(this);
  while (!sessions_.empty()) {
    DevToolsAgentHostClient* client = (*sessions_.begin())->client();
    DetachClient(client);
     client->AgentHostClosed(this);
   }
 }
