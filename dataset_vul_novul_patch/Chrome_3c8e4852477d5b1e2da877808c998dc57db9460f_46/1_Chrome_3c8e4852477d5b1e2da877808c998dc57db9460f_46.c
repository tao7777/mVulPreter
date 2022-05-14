void SharedWorkerDevToolsAgentHost::AttachSession(DevToolsSession* session) {
   session->AddHandler(std::make_unique<protocol::InspectorHandler>());
   session->AddHandler(std::make_unique<protocol::NetworkHandler>(GetId()));
   session->AddHandler(std::make_unique<protocol::SchemaHandler>());
  session->SetRenderer(GetProcess(), nullptr);
   if (state_ == WORKER_READY)
     session->AttachToAgent(EnsureAgent());
 }
