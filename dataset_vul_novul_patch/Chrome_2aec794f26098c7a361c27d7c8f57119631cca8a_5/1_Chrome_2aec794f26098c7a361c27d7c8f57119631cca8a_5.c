 void DevToolsAgentHostImpl::AttachClient(DevToolsAgentHostClient* client) {
   if (SessionByClient(client))
     return;
  InnerAttachClient(client);
 }
