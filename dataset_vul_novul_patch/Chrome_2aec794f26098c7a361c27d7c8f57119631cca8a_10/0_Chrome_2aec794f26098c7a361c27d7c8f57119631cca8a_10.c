 DevToolsSession::DevToolsSession(DevToolsAgentHostImpl* agent_host,
                                 DevToolsAgentHostClient* client,
                                 bool restricted)
     : binding_(this),
       agent_host_(agent_host),
       client_(client),
      restricted_(restricted),
       process_host_id_(ChildProcessHost::kInvalidUniqueID),
       host_(nullptr),
       dispatcher_(new protocol::UberDispatcher(this)),
      weak_factory_(this) {
  dispatcher_->setFallThroughForNotFound(true);
}
