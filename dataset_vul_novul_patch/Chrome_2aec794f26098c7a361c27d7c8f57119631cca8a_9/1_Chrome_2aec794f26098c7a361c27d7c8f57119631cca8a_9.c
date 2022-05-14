void DevToolsAgentHostImpl::InnerAttachClient(DevToolsAgentHostClient* client) {
   scoped_refptr<DevToolsAgentHostImpl> protect(this);
  DevToolsSession* session = new DevToolsSession(this, client);
   sessions_.insert(session);
   session_by_client_[client].reset(session);
  AttachSession(session);
   if (sessions_.size() == 1)
     NotifyAttached();
   DevToolsManager* manager = DevToolsManager::GetInstance();
   if (manager->delegate())
     manager->delegate()->ClientAttached(this, client);
 }
