 void DevToolsSession::AddHandler(
     std::unique_ptr<protocol::DevToolsDomainHandler> handler) {
   handler->Wire(dispatcher_.get());
  handler->SetRenderer(process_host_id_, host_);
   handlers_[handler->name()] = std::move(handler);
 }
