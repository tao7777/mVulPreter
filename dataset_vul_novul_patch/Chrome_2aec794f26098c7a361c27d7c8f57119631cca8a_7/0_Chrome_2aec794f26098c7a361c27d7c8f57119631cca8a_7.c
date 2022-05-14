 void DevToolsAgentHostImpl::ForceAttachClient(DevToolsAgentHostClient* client) {
   if (SessionByClient(client))
     return;
   scoped_refptr<DevToolsAgentHostImpl> protect(this);
   if (!sessions_.empty())
    ForceDetachAllSessions();
   DCHECK(sessions_.empty());
  InnerAttachClient(client, false /* restricted */);
 }
