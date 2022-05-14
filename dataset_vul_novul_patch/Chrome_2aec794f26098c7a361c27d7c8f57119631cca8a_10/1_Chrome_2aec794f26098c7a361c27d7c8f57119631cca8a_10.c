 DevToolsSession::DevToolsSession(DevToolsAgentHostImpl* agent_host,
                                 DevToolsAgentHostClient* client)
     : binding_(this),
       agent_host_(agent_host),
       client_(client),
       process_host_id_(ChildProcessHost::kInvalidUniqueID),
       host_(nullptr),
       dispatcher_(new protocol::UberDispatcher(this)),
      weak_factory_(this) {
  dispatcher_->setFallThroughForNotFound(true);
}
