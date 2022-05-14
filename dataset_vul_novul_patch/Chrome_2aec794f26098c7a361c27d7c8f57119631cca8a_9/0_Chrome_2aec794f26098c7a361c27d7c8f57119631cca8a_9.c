void DevToolsAgentHostImpl::InnerAttachClient(DevToolsAgentHostClient* client) {
bool DevToolsAgentHostImpl::InnerAttachClient(DevToolsAgentHostClient* client,
                                              bool restricted) {
   scoped_refptr<DevToolsAgentHostImpl> protect(this);
  DevToolsSession* session = new DevToolsSession(this, client, restricted);
   sessions_.insert(session);
   session_by_client_[client].reset(session);
  if (!AttachSession(session)) {
    sessions_.erase(session);
    session_by_client_.erase(client);
    return false;
  }

   if (sessions_.size() == 1)
     NotifyAttached();
   DevToolsManager* manager = DevToolsManager::GetInstance();
   if (manager->delegate())
     manager->delegate()->ClientAttached(this, client);
  return true;
 }
