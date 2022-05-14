void DevToolsAgentHost::DetachAllClients() {
  if (!g_devtools_instances.IsCreated())
    return;

   DevToolsMap copy = g_devtools_instances.Get();
   for (DevToolsMap::iterator it(copy.begin()); it != copy.end(); ++it) {
     DevToolsAgentHostImpl* agent_host = it->second;
    agent_host->ForceDetachAllSessions();
   }
 }
