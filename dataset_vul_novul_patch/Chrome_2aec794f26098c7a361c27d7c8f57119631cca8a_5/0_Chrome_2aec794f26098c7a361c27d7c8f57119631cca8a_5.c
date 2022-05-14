 void DevToolsAgentHostImpl::AttachClient(DevToolsAgentHostClient* client) {
   if (SessionByClient(client))
     return;
  InnerAttachClient(client, false /* restricted */);
}

bool DevToolsAgentHostImpl::AttachRestrictedClient(
    DevToolsAgentHostClient* client) {
  if (SessionByClient(client))
    return false;
  return InnerAttachClient(client, true /* restricted */);
 }
