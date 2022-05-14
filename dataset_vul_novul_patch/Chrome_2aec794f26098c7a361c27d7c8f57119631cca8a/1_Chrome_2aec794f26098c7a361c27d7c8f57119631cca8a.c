 void ExtensionDevToolsClientHost::AgentHostClosed(
     DevToolsAgentHost* agent_host) {
   DCHECK(agent_host == agent_host_.get());
   SendDetachedEvent();
   delete this;
 }
