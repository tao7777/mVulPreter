void DevToolsAgentHostImpl::ForceDetachAllClients() {
   scoped_refptr<DevToolsAgentHostImpl> protect(this);
  while (!session_by_client_.empty()) {
    DevToolsAgentHostClient* client = session_by_client_.begin()->first;
    InnerDetachClient(client);
     client->AgentHostClosed(this);
   }
 }
